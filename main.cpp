#include <iostream>
#include <cstdlib>
#include <thread>
#include <chrono>

#include "sciplot/sciplot.hpp"
#include <nlohmann/json.hpp>
#include <curl/curl.h>

using namespace sciplot;
using json = nlohmann::json;

void show_info() {
    std::cout << "######################################################" << std::endl;
    std::cout << "########       Welcome to visualAPI !!!       ########" << std::endl;
    std::cout << "######################################################" << std::endl;
    std::cout << std::endl;
    std::cout
            << "Description: \n\tvisualAPI uses Chess.com public API to visualize some popular statistics available.\n"
               "\tPresented data is accurate as it is downloaded at the moment you run your program."
               "\n\tPlots and diagrams are generated using gnuplot and sciplot libraries.";
    std::cout << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "\tPress 1 to see best server player statistics." << std::endl;
    std::cout << "\tPress 2 to see 10 best server players." << std::endl;
    std::cout << "\tPress 3 to see number of players by top FIDE countries." << std::endl;
    std::cout << "\tPress 4 to see number of players by FIDE title." << std::endl;
    std::cout << "\n\tPress 0 to quit program." << std::endl;
}

int get_user_input() {
    int user_option;

    std::cout << "\nChosen option: ";
    std::cin >> user_option;
    std::cout << "Processing.." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    return user_option;
}

void get_page(const char *url, const char *file_name) {
    CURL *easyhandle = curl_easy_init();

    curl_easy_setopt(easyhandle, CURLOPT_URL, url);

    FILE *file = fopen(file_name, "w");

    curl_easy_setopt(easyhandle, CURLOPT_WRITEDATA, file);
    curl_easy_perform(easyhandle);
    curl_easy_cleanup(easyhandle);

    fclose(file);

}

int main() {
    // Show main program information to the user and get option from user.
    show_info();
    int option = get_user_input();

    // Prepare urls for downloading data from Chess.com API.
    std::string url_best_player = "https://api.chess.com/pub/player/hikaru/stats";
    std::string url_best_players_by_cat = "https://api.chess.com/pub/leaderboards";
    std::string url_by_country = "https://api.chess.com/pub/country/";
    std::string url_by_title = "https://api.chess.com/pub/titled/";

    std::string top_11_fide_countries[11] = {"US", "RU", "RC", "IN", "UA",
                                             "AZ", "FR", "AM", "DE", "ES", "PL"};
    std::string fide_titles[6] = {"GM", "IM", "FM", "WGM", "WIM", "WFM"};

    // Download data with curl depending on user input.
    if (option == 0) {
        std::cout << "Exiting program." << std::endl;
        exit(0);
    } else if (option == 1) {
        std::cout << "Downloading data of best server player.." << std::endl;
        get_page(url_best_player.c_str(), "../data1.json");
    } else if (option == 2) {
        std::cout << "Downloading data of 10 best server players.." << std::endl;
        get_page(url_best_players_by_cat.c_str(), "../data1.json");
    } else if (option == 3) {
        for (int i = 0; i < 11; i++) {
            std::string i_url = url_by_country + top_11_fide_countries[i] + "/players";
            std::string i_file = "../data" + std::to_string(i + 1) + ".json";
            std::cout << "Downloading data for coutry - " + top_11_fide_countries[i] << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            get_page(i_url.c_str(), i_file.c_str());
        }
    } else if (option == 4) {
        for (int i = 0; i < 6; i++) {
            std::string i_url = url_by_title + fide_titles[i];
            std::string i_file = "../data" + std::to_string(i + 1) + ".json";
            std::cout << "Downloading data for title - " + fide_titles[i] << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            get_page(i_url.c_str(), i_file.c_str());
        }
    } else {
        std::cout << "Incorrect option passed. Exiting program." << std::endl;
        exit(1);
    }

    // Get data from downloaded json files.
    json basic_json;
    std::ifstream input("../data1.json");
    input >> basic_json;

    std::cout << "Contents of json file:" << std::endl;
    std::cout << basic_json.dump(2) << std::endl;

    // TODO: Visualize it in form of a graph.
    // TODO: Add saving file by option with number to different folder.

    // Plot option 1.
    Strings names = {"Hikaru"};
    Vec ratings = {basic_json["chess_bullet"]["best"]["rating"],
                   basic_json["chess_blitz"]["best"]["rating"],
                   basic_json["chess_rapid"]["best"]["rating"]};
    Strings game_results = {"win", "loss", "draw"};
    Strings game_types = {"bullet", "blitz", "rapid"};
    Vec bullet_games = {basic_json["chess_bullet"]["record"]["win"],
                        basic_json["chess_bullet"]["record"]["loss"],
                        basic_json["chess_bullet"]["record"]["draw"]};

    Plot plot1;
    plot1.legend().atOutsideBottom();
    plot1.legend().title("Rating of " + names[0] + " in different game types");
    plot1.legend().titleFontSize(16);
    plot1.size(1200, 800);

    plot1.ylabel("Rating");

    plot1.drawBoxes(game_types, ratings)
            .fillSolid()
            .fillIntensity(0.5)
            .borderShow()
            .labelNone();;

    plot1.boxWidthRelative(0.75);
    plot1.autoclean(false);

    Plot plot2;
    plot2.legend().atOutsideBottom();
    plot2.legend().title("Number of games " + names[0] + " by result");
    plot2.legend().titleFontSize(16);
    plot2.size(1200, 800);

    plot2.ylabel("Number of games");

    plot2.drawBoxes(game_results, bullet_games)
            .fillSolid()
            .fillIntensity(0.5)
            .borderShow()
            .labelNone();;

    plot2.boxWidthRelative(0.75);
    plot2.autoclean(false);

    // Use the previous plots as sub-figures in a larger 1x2 figure.
    Figure fig = {{ plot1, plot2 }};

    fig.size(1200, 800);
    fig.title("Hikaru chess player statistics");
    fig.fontSize(22);
    fig.palette("dark2");

    fig.show();

    fig.save("../plot.png");

    return 0;
}
