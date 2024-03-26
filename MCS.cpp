#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <sstream>

#define getRandomIndex(x) (x[rand() % x.size()])

using namespace std;

// Constants
const bool INFECTED = true;
const bool CLEAN = false;

// Simulation parameters
int infection_threshold = 1;
int network_size = 20;
int repair_limit = 5;
int simulation_runs = 10000;
float transmission_rate = .1;

// Probability cache
vector<float> infection_chance;

// Utility function for Bernoulli trials
bool trial(float probability) {
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX) < probability;
}

// Function to initialize probabilities
void init_probabilities() {
    srand(static_cast<unsigned>(time(nullptr)));

    float prob = 1.0;
    for (int i = 0; i <= network_size; ++i) {
        infection_chance.push_back(1.0 - prob);
        prob *= (1.0 - transmission_rate);
    }
}

// Function to run the Monte Carlo simulation
void simulate(float& avg_days, float& infection_probability, float& avg_infections) {
    vector<bool> system_state(network_size, CLEAN);
    vector<bool> infection_history(network_size, false);

    auto findIndices = [&system_state](bool search_for) -> vector<int> {
        vector<int> indices;
        for (int i = 0; i < system_state.size(); ++i) {
            if (system_state[i] == search_for) {
                indices.push_back(i);
            }
        }
        return indices;
    };

    for (int count = infection_threshold; count--;) {
        auto healthy_indices = findIndices(CLEAN);
        system_state[getRandomIndex(healthy_indices)] = INFECTED;
    }

    bool in_progress = true;
    int day_counter = 0;
    float total_infections = 0;

    while (in_progress) {
        ++day_counter;
        float current_chance = infection_chance[count(system_state.begin(), system_state.end(), INFECTED)];

        for (auto index : findIndices(CLEAN)) {
            if (trial(current_chance)) {
                system_state[index] = INFECTED;
            }
        }

        total_infections += count(system_state.begin(), system_state.end(), INFECTED);

        for (auto index : findIndices(INFECTED)) {
            infection_history[index] = true;
        }

        for (int repairs = repair_limit; repairs--;) {
            auto infected_indices = findIndices(INFECTED);
            if (infected_indices.empty()) {
                in_progress = false;
                break;
            }
            system_state[getRandomIndex(infected_indices)] = CLEAN;
        }
    }

    avg_days += day_counter;
    infection_probability += count(infection_history.begin(), infection_history.end(), true) == network_size;
    avg_infections += count(infection_history.begin(), infection_history.end(), true);
}

// Function to get user input
template<typename T>
T get_user_input(const string& prompt, T default_value) {
    cout << prompt << " (default = " << default_value << "): ";
    string input;
    getline(cin, input);
    stringstream input_stream(input);
    T value;
    if (input_stream >> value) return value;
    return default_value;
}

// Main entry point
int main() {
  
    std::cout << "Press enter to choose default value. \n\n";
  
    init_probabilities();

    simulation_runs = get_user_input<int>("Enter the number of simulations", simulation_runs);
    transmission_rate = get_user_input<float>("Enter the virus transmission rate", transmission_rate);
    infection_threshold = get_user_input<int>("Enter the initial infection count", infection_threshold);
    network_size = get_user_input<int>("Enter the size of computer network", network_size);
    repair_limit = get_user_input<int>("Enter the daily repair limit", repair_limit);

    std::cout << "\n\n";

    float total_days = 0, total_prob = 0, total_avg_infection = 0;

    clock_t start_time = clock();
    for (int i = 0; i < simulation_runs; ++i) {
        simulate(total_days, total_prob, total_avg_infection);
    }

    float time_elapsed = static_cast<float>(clock() - start_time) / CLOCKS_PER_SEC;

    cout << "Simulation complete." << endl;
    cout << "Time elapsed: " << time_elapsed << " seconds" << endl;
    cout << "Average days to clean network: " << total_days / simulation_runs << endl;
    cout << "Probability all computers get infected at least once: " << total_prob / simulation_runs << endl;
    cout << "Average number of computers infected: " << total_avg_infection / simulation_runs << endl;
    return 0;
    }
