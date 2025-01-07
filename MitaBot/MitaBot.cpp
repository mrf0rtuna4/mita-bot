#include "Mita.h"
#include <dpp/dpp.h>
#include <cstdlib>
#include <iostream>
#include <string>

std::string get_env_variable(const char* var_name) {
    char* buffer = nullptr;
    size_t size = 0;
    if (_dupenv_s(&buffer, &size, var_name) == 0 && buffer != nullptr) {
        std::string value(buffer);
        free(buffer);
        return value;
    }
    return {};
}

const std::string BOT_TOKEN = get_env_variable("MITA_TOKEN");

int main() {
    if (BOT_TOKEN.empty()) {
        std::cerr << BOT_TOKEN << std::endl;
        return 1;
    }

    dpp::cluster bot(BOT_TOKEN);

    bot.on_log(dpp::utility::cout_logger());

    bot.on_ready([&bot](const dpp::ready_t& event) {
        if (dpp::run_once<struct register_bot_commands>()) {
            std::vector<dpp::slashcommand> commands{
                { "ping", "Ping pong!", bot.me.id }
            };

            bot.global_bulk_command_create(commands);
        }
        });

    bot.on_slashcommand([](const dpp::slashcommand_t& event) -> dpp::task<void> {
        if (event.command.get_command_name() == "ping") {
            co_await event.co_reply("Pong!");
        }
        co_return;
        });

    bot.start(dpp::st_wait);

    return 0;
}
