#include <iostream>
#include <cstdlib>
#include <thread>
#include <chrono>

#include "sciplot/sciplot.hpp"
#include <nlohmann/json.hpp>
#include <curl/curl.h>
#include <set>

using namespace sciplot;
using json = nlohmann::json;

void show_info();

int get_user_input();

void get_page(const char *url, const char *file_name);

void exit_program();

void get_json_file(std::string url, int file_number);

json get_json_object();

void get_best_player_plot();

void get_best_50_players();

void get_player_stats();

void get_players_by_title_plot();

int main() {
    // Show main program information to the user and get option from user.
    show_info();
    int option = get_user_input();

    // Prepare urls for downloading data from Chess.com API.
    std::string url_best_player = "https://api.chess.com/pub/player/hikaru/stats";
    std::string url_best_players_by_cat = "https://api.chess.com/pub/leaderboards";
    std::string url_chosen_player = "https://api.chess.com/pub/player/";
    std::string url_by_title = "https://api.chess.com/pub/titled/";

    std::string fide_titles[6] = {"GM", "IM", "FM", "WGM", "WIM", "WFM"};

    if (option == 0) {
        exit_program();
    } else if (option == 1) {
        // Download json file.
        get_json_file(url_best_player, 1);
        get_best_player_plot();
    } else if (option == 2) {
        get_json_file(url_best_players_by_cat, 1);
        get_best_50_players();
    } else if (option == 3) {
        std::string nickname;
        std::cout << "Enter nickname of player (ex. DanielNaroditsky|MagnusCarlsen|Hikaru): ";
        std::cin >> nickname;
        std::transform(nickname.begin(), nickname.end(), nickname.begin(),
                       [](unsigned char c){ return std::tolower(c); });
        std::string i_url = url_chosen_player + nickname;

        get_json_file(i_url, 1);
        get_player_stats();
    } else if (option == 4) {
        for (int i = 0; i < 6; i++) {
            std::string i_url = url_by_title + fide_titles[i];
            std::cout << "Downloading data for title - " + fide_titles[i] << "\t";
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            get_json_file(i_url, i + 1);
        }
        get_players_by_title_plot();
    } else {
        std::cout << "Incorrect option passed." << std::endl;
        exit_program();
    }

    return 0;
}

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
    std::cout << "\tPress 2 to see 50 best server players." << std::endl;
    std::cout << "\tPress 3 to check stats of chosen player." << std::endl;
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

void exit_program() {
    std::cout << "Exiting program.." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    exit(0);
}

std::string get_last_chars(std::string input, int n) {
    int inputSize = input.size();
    return (n > 0 && inputSize > n) ? input.substr(inputSize - n) : "";
}

void get_json_file(std::string url, int file_number) {
    std::cout << "Downloading JSON file.." << std::endl;
    std::string i_file = "../data" + std::to_string(file_number) + ".json";
    get_page(url.c_str(), i_file.c_str());
}

json get_json_object() {
    // Get data from downloaded json files.
    json basic_json;
    std::ifstream input("../data1.json");
    input >> basic_json;

    std::cout << "Contents of json file:" << std::endl;
    std::cout << basic_json.dump(2) << std::endl;

    return basic_json;
}

json get_json_object_by_number(int num) {
    // Get data from downloaded json files.
    json basic_json;
    std::string i_str = "../data" + std::to_string(num) + ".json";
    std::ifstream input(i_str);
    input >> basic_json;

    return basic_json;
}

void get_best_player_plot() {
    // Get json object to work on.
    json basic_json = get_json_object();

    // Plot option 1.
    Strings names = {"Hikaru"};
    Strings game_results = {"win", "loss", "draw"};
    Strings game_types = {"bullet", "blitz", "rapid"};
    Vec ratings = {basic_json["chess_bullet"]["best"]["rating"],
                   basic_json["chess_blitz"]["best"]["rating"],
                   basic_json["chess_rapid"]["best"]["rating"]};
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
            .labelNone();

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
    Figure fig = {{plot1, plot2}};

    fig.size(1200, 800);
    fig.title("Hikaru chess player statistics");
    fig.fontSize(22);
    fig.palette("dark2");

    fig.show();

    fig.save("../plots/plot1.png");
}

void get_best_50_players() {
    // Get json object to work on.
    json basic_json = get_json_object();

    // Plot option 2.
    json players = {};
    for (int i = 0; i < 50; i++) {
        players.push_back(basic_json["live_blitz"][i]);
    }
    Strings names = {};
    Strings countries = {};
    std::set<std::string> unique_countries = {};
    std::vector<int> ratings = {};
    std::vector<int> rank = {};
    for (int i = 0; i < 50; i++) {
        names.push_back(players[i]["username"]);
        countries.push_back(get_last_chars(players[i]["country"], 2));
        unique_countries.insert(countries[i]);
        ratings.push_back(players[i]["score"]);
        rank.push_back(i + 1);
    }

    std::map<std::string, int> country_freq_map = {};
    Vec country_rank((double) 0, 50);

    for (const auto& country : unique_countries) {
        country_freq_map.insert({country, 0});
    }
    for (int i = 0; i < 50; i++) {
        if (unique_countries.find(countries[i]) != unique_countries.end()) {
            country_freq_map[countries[i]] += 1;
            country_rank[i] += 1;
        }
    }

    Strings x_values = {"AM", "BR", "BY", "CZ", "DE", "EG", "FM", "FR", "GE", "HU", "IL", "IN", "KP", "MD",
                    "MX", "NL", "PE", "PL", "PW", "RS", "RU", "ST", "UA", "US", "VN"};
    Vec y_values = {1, 1, 1, 1, 4, 1, 1, 1, 1, 1, 2, 3, 1, 1, 1, 1, 2, 1, 1, 1, 5, 1, 3, 12, 1};

    Plot plot1;
    plot1.legend().atOutsideBottom();
    plot1.legend().title("Rating of top 50 players");
    plot1.legend().titleFontSize(16);
    plot1.size(1200, 800);

    plot1.ylabel("Rating");

    plot1.drawBoxes(rank, ratings)
            .fillSolid()
            .fillIntensity(0.5)
            .borderShow()
            .labelNone();

    plot1.boxWidthRelative(0.75);
    plot1.autoclean(false);

    Plot plot2;
    plot2.legend().atOutsideBottom();
    plot2.legend().title("Top players by country");
    plot2.legend().titleFontSize(16);
    plot2.size(1200, 800);

    plot2.ylabel("Number of players");

    plot2.drawBoxes(x_values, y_values)
            .fillSolid()
            .fillIntensity(0.5)
            .borderShow()
            .labelNone();

    plot2.boxWidthRelative(0.75);
    plot2.autoclean(false);

    plot1.show();
    plot2.show();

    plot1.save("../plots/plot2_1.png");
    plot2.save("../plots/plot2_2.png");
}

void get_player_stats() {
    // Get json object to work on.
    json basic_json = get_json_object();

    std::cout << "\nStats for " << basic_json["username"] << std::endl;
    std::cout << "\tName: " << basic_json["name"] << std::endl;
    std::cout << "\tIs verified? " << basic_json["verified"] << std::endl;
    std::cout << "\tCountry: " << get_last_chars(basic_json["country"], 2)<< std::endl;
    std::cout << "\tLocation: " << basic_json["location"] << std::endl;
    std::cout << "\tTitle: " << basic_json["title"] << std::endl;
    std::cout << "\tFollowers: " << basic_json["followers"] << std::endl;
    std::cout << "\tIs streamer? " << basic_json["is_streamer"] << std::endl;
    std::cout << "\tTwitch url: " << basic_json["twitch_url"] << std::endl;
}

void get_players_by_title_plot() {
    // Get json objects to work on.
    Strings fide_titles = {"GM", "IM", "FM", "WGM", "WIM", "WFM"};
    std::vector<int> fide_titles_no = {};
    std::string placeholder;

    for (int i = 0; i < 6; i++) {
        json basic_json = get_json_object_by_number(i + 1);
        fide_titles_no.push_back(basic_json["players"].size());
    }

    // Plot option 4.
    Plot plot1;
    plot1.legend().atOutsideBottom();
    plot1.legend().title("Number of players by FIDE title");
    plot1.legend().titleFontSize(16);
    plot1.size(1200, 800);

    plot1.ylabel("Number of players");

    plot1.drawBoxes(fide_titles, fide_titles_no)
            .fillSolid()
            .fillIntensity(0.5)
            .borderShow()
            .labelNone();

    plot1.boxWidthRelative(0.75);
    plot1.autoclean(false);

    plot1.show();

    plot1.save("../plots/plot4.png");
}
