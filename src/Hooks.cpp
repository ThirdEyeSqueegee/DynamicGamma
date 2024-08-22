#include "Hooks.h"

#include "GammaController.h"
#include "Settings.h"

namespace Hooks
{
    void Install() noexcept
    {
        stl::write_thunk_call<MainUpdate>();
        logger::info("Installed main update hook");
    }

    static constexpr auto main_menu{ RE::MainMenu::MENU_NAME };
    static constexpr auto map_menu{ RE::MapMenu::MENU_NAME };
    static constexpr auto mist_menu{ RE::MistMenu::MENU_NAME };
    static constexpr auto loading_menu{ RE::LoadingMenu::MENU_NAME };

    i32 MainUpdate::Thunk() noexcept
    {
        const auto result{ func() };

        const auto now{ std::chrono::steady_clock::now() };

        if (now - GammaController::last_run <= std::chrono::milliseconds(Settings::refresh_rate)) {
            return result;
        }

        GammaController::last_run = now;

        const auto current_gamma{ GammaController::gamma_setting->GetFloat() };
        if (const auto ui{ RE::UI::GetSingleton() }) {
            if (ui->IsMenuOpen(main_menu) || ui->IsMenuOpen(map_menu) || ui->IsMenuOpen(mist_menu) || ui->IsMenuOpen(loading_menu)) {
                if (current_gamma != GammaController::original_gamma) {
                    logger::debug("Open menu detected, resetting gamma");
                    const RE::BSReadLockGuard guard{ GammaController::lock };
                    GammaController::gamma_setting->data.f = GammaController::original_gamma;
                }
                return result;
            }
        }
        if (GammaController::control_global->value == 0.0) {
            if (current_gamma != GammaController::original_gamma) {
                logger::debug("Global set to 0, resetting gamma");
                const RE::BSReadLockGuard guard{ GammaController::lock };
                GammaController::gamma_setting->data.f = GammaController::original_gamma;
            }
            return result;
        }
        if (const auto calendar{ RE::Calendar::GetSingleton() }) {
            if (const auto player{ RE::PlayerCharacter::GetSingleton() }; player->Is3DLoaded()) {
                const HourAndMinute hour_and_minute{ .hour = static_cast<u8>(calendar->GetHour()), .minute = static_cast<u8>(calendar->GetMinutes()) };
                logger::debug("Current time: {}", hour_and_minute);
                logger::debug("Current gamma: {:.2f}", current_gamma);

                auto map{ &Settings::exterior_map };
                auto keys{ &Settings::exterior_keys_sorted };
                if (const auto parent_cell{ player->GetParentCell() }) {
                    if (parent_cell->IsInteriorCell()) {
                        logger::debug("Player is in interior cell {}", parent_cell->GetName());
                        map  = &Settings::interior_map;
                        keys = &Settings::interior_keys_sorted;
                    }
                }
                else {
                    logger::debug("Player is in exterior cell");
                }

                if (const auto lower_bound{ std::ranges::lower_bound(*keys, hour_and_minute, pred) }; lower_bound != keys->end()) {
                    if (const auto new_gamma{ map->at(*lower_bound) }; current_gamma != new_gamma) {
                        const RE::BSReadLockGuard guard{ GammaController::lock };
                        GammaController::gamma_setting->data.f = new_gamma;
                    }
                }
            }
        }

        return result;
    }

} // namespace Hooks
