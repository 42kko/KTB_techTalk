#include <dpp/dpp.h>
#include <string>
#include <cstdlib> // getenv 사용
#include <dpp/nlohmann/json.hpp>
using namespace std;

// 구조체 정의
struct Say {
    string author;
    string authorProfile;
    string message;
};

const string BOT_TOKEN = getenv("BOT_TOKEN") ? getenv("BOT_TOKEN") : "";
const string Channel_ID = getenv("CHANNEL_ID") ? getenv("CHANNEL_ID") : "";

int main() {
    // 환경변수가 설정되지 않았을 경우 오류 출력
    if (BOT_TOKEN.empty() || Channel_ID.empty()) {
        cerr << "Error: BOT_TOKEN or CHANNEL_ID is not set!" << endl;
        return 1;
    }

    dpp::cluster bot(BOT_TOKEN);

    bot.on_log(dpp::utility::cout_logger());

    // 슬래쉬 명령어 작동 로직
    bot.on_slashcommand([&bot](const dpp::slashcommand_t& event) {
        if (event.command.get_command_name() == "ping") {
            event.reply("Pong!");
        }
        else if (event.command.get_command_name() == "명언") {
            event.thinking();
            bot.request("https://korean-advice-open-api.vercel.app/api/advice", dpp::m_get, [event](const dpp::http_request_completion_t &cc) {
                Say tmp;
                nlohmann::json jsonData = nlohmann::json::parse(cc.body);
                tmp.author = jsonData["author"].get<string>();
                tmp.authorProfile = jsonData["authorProfile"].get<string>();
                tmp.message = jsonData["message"].get<string>();
                event.edit_response("**명언:** " + tmp.message + "\n- " + tmp.author + " - " + tmp.authorProfile);
            });
        }
    });

    bot.on_ready([&bot](const dpp::ready_t& event) {
        // 타이머 옵션
        bot.start_timer([&bot](const dpp::timer& timer) {
            bot.request("https://korean-advice-open-api.vercel.app/api/advice", dpp::m_get, [&bot](const dpp::http_request_completion_t& cc) {
                Say tmp;
                nlohmann::json jsonData = nlohmann::json::parse(cc.body);
                tmp.author = jsonData["author"].get<string>();
                tmp.authorProfile = jsonData["authorProfile"].get<string>();
                tmp.message = jsonData["message"].get<string>();

                bot.message_create(dpp::message(Channel_ID, "**명언:** " + tmp.message + "\n- " + tmp.author + " - " + tmp.authorProfile));
            });
        }, 10000);

        // 슬래쉬 명령어 등록
        if (dpp::run_once<struct clear_and_register_commands>()) {
            bot.global_command_create(dpp::slashcommand("ping", "Ping pong!", bot.me.id));
            bot.global_command_create(dpp::slashcommand("명언", "랜덤한 명언 가져오기!", bot.me.id));
        }
    });

    bot.start(dpp::st_wait);
}