#include <dpp/dpp.h>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
using namespace std;

const string BOT_TOKEN = "";
const string Channel_ID = "";

typedef struct Say {
    string author;
    string authorProfile;
    string message;
} say;

// CURL 응답 데이터를 저장하는 데 사용될 버퍼
static string responseBuffer;

// CURL의 WriteCallback 함수
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t totalSize = size * nmemb;
    responseBuffer.append((char*)contents, totalSize);
    return totalSize;
}

say get_api(string type, string address, string token) {
    say ret;
    CURL* curl;
    CURLcode res;

    curl = curl_easy_init(); // CURL 초기화
    if (curl) {
        struct curl_slist* headers = NULL;
        responseBuffer.clear(); // 응답 버퍼 초기화

        // Authorization 헤더 추가
        string authHeader = "Authorization: Bearer " + token;
        headers = curl_slist_append(headers, authHeader.c_str());

        // CURL 옵션 설정
        curl_easy_setopt(curl, CURLOPT_URL, address.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL);

        // 요청 실행
        res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            // 응답 처리: JSON 파싱
            try {
                nlohmann::json jsonData = nlohmann::json::parse(responseBuffer); // JSON 응답 파싱
                ret.author = jsonData["author"].get<string>();
                ret.authorProfile = jsonData["authorProfile"].get<string>();
                ret.message = jsonData["message"].get<string>();
            } catch (nlohmann::json::parse_error& e) {
                cerr << "JSON parsing error: " << e.what() << endl;
            }
        } else {
            cerr << "CURL error: " << curl_easy_strerror(res) << endl;
        }

        // CURL 정리
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    return ret;
}

int main() {
    dpp::cluster bot(BOT_TOKEN);

    bot.on_log(dpp::utility::cout_logger());

    //슬래쉬 명령어 작동로직
    bot.on_slashcommand([&bot](const dpp::slashcommand_t& event) {
        if (event.command.get_command_name() == "ping") {
            event.reply("Pong!");
        } 
        else if (event.command.get_command_name() == "명언") {
            say saying = get_api("Get", "https://korean-advice-open-api.vercel.app/api/advice", "");
            event.reply("**명언:** " + saying.message + "\n- " + saying.author + " - " + saying.authorProfile);
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
        }, 10);

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