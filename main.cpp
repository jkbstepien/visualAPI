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
    if (option == 1) {
        get_page(url_best_player.c_str(), "../data1.json");
    } else if (option == 2) {
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

    // TODO: Prepare data using json.
//    json j;
//    std::ifstream i("data1.json");
//    i >> j;
//
//    std::cout << "Downloading to data.json.." << std::endl;
//    std::this_thread::sleep_for(std::chrono::milliseconds(100));
//    std::cout << j.dump(4) << std::endl;

    // TODO: Visualize it in form of a graph.

//    // Create a vector with the xtic labels for the boxes
//    Strings names = {"John", "Peter", "Thomas", "Marta"};
//
//    // Create a vector with the y values for the boxes
//    Vec ages = {44, 27, 35, 20};
//
//    // Create a vector with the xwidth values for the boxes
//    Vec experiences = {0.8, 0.4, 0.7, 0.9};
//
//    // Create a Plot object
//    Plot plot;
//
//    // Set the legend to the top left corner of the plot
//    plot.legend().atTopLeft();
//
//    // Set the y label and its range
//    plot.ylabel("Age");
//    plot.yrange(0.0, 50);
//    plot.size(1200, 800);
//
//    // Plot the boxes using y values.
//    plot.drawBoxes(names, ages, experiences)
//            .fillSolid()
//            .fillColor("pink")
//            .fillIntensity(0.5)
//            .borderShow()
//            .labelNone();
//
//    // Adjust the relative width of the boxes
//    plot.boxWidthRelative(0.75);
//
//    plot.autoclean(false);
//
//    // Show the plot in a pop-up window
//    plot.show();
//
//    // Save the plot to a PDF file
//    plot.save("example-boxes-ticklabels.pdf");
//
//    return 0;
}
