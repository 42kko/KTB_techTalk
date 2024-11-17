#include <dpp/dpp.h>
#include <string>
#include <dpp/nlohmann/json.hpp>
using namespace std;

const string BOT_TOKEN = "";
const string Channel_ID = "";

typedef struct Say {
    string author;
    string authorProfile;
    string message;
} say;

int main() {
    dpp::cluster bot(BOT_TOKEN);

    bot.on_log(dpp::utility::cout_logger());

    //슬래쉬 명령어 작동로직
    bot.on_slashcommand([&bot](const dpp::slashcommand_t& event) {
        if (event.command.get_command_name() == "ping") {
            event.reply("Pong!");
        } 
        else if (event.command.get_command_name() == "명언") {
            event.thinking();
            bot.request("https://korean-advice-open-api.vercel.app/api/advice", dpp::m_get, [event](const dpp::http_request_completion_t &cc) {
                say tmp;
                nlohmann::json jsonData = nlohmann::json::parse(cc.body);
                tmp.author = jsonData["author"].get<string>();
                tmp.authorProfile = jsonData["authorProfile"].get<string>();
                tmp.message = jsonData["message"].get<string>();
                event.edit_response("**명언:** " + tmp.message + "\n- " + tmp.author + " - " + tmp.authorProfile);
            });
        }
    });

    bot.on_ready([&bot](const dpp::ready_t& event) {

        //타이머 옵션
        bot.start_timer([&bot](const dpp::timer& timer){
            bot.request("https://korean-advice-open-api.vercel.app/api/advice", dpp::m_get, [&bot](const dpp::http_request_completion_t& cc) {
                say tmp;
                nlohmann::json jsonData = nlohmann::json::parse(cc.body);
                tmp.author = jsonData["author"].get<string>();
                tmp.authorProfile = jsonData["authorProfile"].get<string>();
                tmp.message = jsonData["message"].get<string>();

                bot.message_create(dpp::message(Channel_ID, "**명언:** " + tmp.message + "\n- " + tmp.author + " - " + tmp.authorProfile));
            });
        }, 10000);

        //슬래쉬 명령어
        if (dpp::run_once<struct clear_and_register_commands>()) {

            /* 명령어 캐시문제 있을경우 기존 명령어를 다 지우는 옵션
            //
            bot.global_commands_get([&bot](const dpp::confirmation_callback_t& callback) {
                if (callback.is_error()) {
                    cerr << "Failed to fetch commands: " << callback.get_error().message << endl;
                    return;
                }

                auto commands = std::get<dpp::slashcommand_map>(callback.value);
                for (const auto& command : commands) {
                    bot.global_command_delete(command.first, [](const dpp::confirmation_callback_t& delete_callback) {
                        if (delete_callback.is_error()) {
                            cerr << "Failed to delete command: " << delete_callback.get_error().message << endl;
                        } else {
                            cout << "Command deleted successfully!" << endl;
                        }
                    });
                }

                bot.global_command_create(dpp::slashcommand("ping", "Ping pong!", bot.me.id));
                bot.global_command_create(dpp::slashcommand("명언", "랜덤한 명언 가져오기!", bot.me.id));
            });
            */

            bot.global_command_create(dpp::slashcommand("ping", "Ping pong!", bot.me.id));
            bot.global_command_create(dpp::slashcommand("명언", "랜덤한 명언 가져오기!", bot.me.id));
        }
    });

    bot.start(dpp::st_wait);
}
