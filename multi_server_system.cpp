#include <bits/stdc++.h>

#define ll long long
#define IDLE 0
#define ACTIVE 1
using namespace std;
double INF = 1e9;


class Process {
public:
    double interarrival_time;
    double service_time;
    double arrival_time;
    Process(double interarrival, double service, double arrival) : interarrival_time(interarrival),
                                                                   service_time(service), arrival_time(arrival) {};
    Process() {
        interarrival_time = 0;
        service_time = 0;
        arrival_time = 0;
    }
};

class Clock {
public:
    double time;
    Clock(int t) : time(t) {};
    Clock() {
        time = 0;
    }
};

class Server {
public:
    bool status;
    Server(bool s) : status(s) {};
    Server() {
        status = IDLE;
    }
};


class Event {
public:
    double value;
    Event(double t) : value(t) {};
    Event() {
        value = INF;
    }
    bool operator==(Event &ev) {
        return this->value == ev.value;
    }
    bool operator==(double v) {
        return this->value == v;
    }
};

Event min_event(vector<Event> events) {
    Event minimum = events[0];
    for (Event e: events) {
        if (e.value < minimum.value)
            minimum = e;
    }
    return minimum;
}

int main() {

    /* ------- Given system data -------- */

//    vector<double> arrival_time = {0, 5, 3, 1, 1};
//    vector<double> service_time = {3, 2, 1, 2, 1};

    vector<double> arrival_time = {0, 2, 4, 4, 2, 2};
    vector<double> service_time = {5, 3, 3, 5, 6, 3};

    /* ------- System components -------- */

    // Data
    vector<Process> processes;
    vector<Process>::iterator current_process;

    // Components
    Server server_1, server_2;
    queue<Process> waiting_queue;
    Event next_arrival_time, departure_1, departure_2;
    Event curr_event;

    // Clocks
    Clock clk, prev_clk;

    // Statics
    double area_under_q = 0, area_under_b = 0;
    double total_delay_time = 0;
    int delayed_processes = 0;


    /* ------- Preprocessing -------- */

    processes.clear();
    vector<double> arrival(arrival_time.size());
    for (int i = 1; i < arrival_time.size(); ++i) {
        arrival[i] = arrival[i - 1] + arrival_time[i];
    }
    for (int i = 0; i < arrival_time.size(); ++i) {
        processes.emplace_back(arrival_time[i], service_time[i], arrival[i]);
    }
    next_arrival_time.value = processes[0].interarrival_time;
    current_process = processes.begin();

    /* ------- Simulation -------- */

    while (true) {
        curr_event = min_event({departure_1, departure_2, next_arrival_time});
        if (curr_event == INF)
            break;

        // update clocks
        prev_clk.time = clk.time;
        clk.time = curr_event.value;

        if (curr_event == departure_1) {
            // update statistics
            area_under_q += (clk.time - prev_clk.time) * (int) waiting_queue.size();
            if (server_2.status)
                area_under_b += clk.time - prev_clk.time;
            else
                area_under_b += (clk.time - prev_clk.time) * 0.5;

            if (!waiting_queue.empty()) { // queue is not empty
                Process next_process = waiting_queue.front();
                waiting_queue.pop();
                total_delay_time += clk.time - next_process.arrival_time;
                departure_1.value =
                        clk.time + next_process.service_time; // add waiting time of the top process
            } else { // no items to insert
                server_1.status = IDLE;
                departure_1.value = INF;
            }
        }
            //----------------------------------D2 is less
        else if (curr_event == departure_2) {
            // update statistics
            area_under_q += (clk.time - prev_clk.time) * (int) waiting_queue.size();
            if (server_1.status)
                area_under_b += clk.time - prev_clk.time;
            else
                area_under_b += (clk.time - prev_clk.time) * 0.5;

            if (!waiting_queue.empty()) { // queue is not empty
                Process next_process = waiting_queue.front();
                waiting_queue.pop();
                total_delay_time += clk.time - next_process.arrival_time;
                //---------------
                if (server_1.status == IDLE) { // insert
                    departure_1.value =
                            clk.time + next_process.service_time; // add waiting time of the top process
                    server_1.status = ACTIVE;
                    if (!waiting_queue.empty()) { // for D2
                        Process next_process = waiting_queue.front();
                        waiting_queue.pop();
                        total_delay_time += clk.time - next_process.arrival_time;
                        departure_2.value =
                                clk.time + next_process.service_time;
                    } else { // server 2 stop
                        server_2.status = IDLE;
                        departure_2.value = INF;
                    }
                }
                    //--------------------------------
                else { // server 1 active server 2 idle
                    departure_2.value =
                            clk.time + next_process.service_time; // add waiting time of the top process
                }
            } else { // no items to insert
                server_2.status = IDLE;
                departure_2.value = INF;
            }
        } else { // curr_time == arrival_time
            if (server_1.status == ACTIVE) {
                area_under_b += (clk.time - prev_clk.time) * 0.5;
            }
            if (server_2.status == ACTIVE) {
                area_under_b += (clk.time - prev_clk.time) * 0.5;
            }
            if (server_1.status == IDLE) { // server is idle
                server_1.status = ACTIVE;
                // update_departure for the current process in the server
                departure_1.value = clk.time + (*current_process).service_time;
            } else if (server_2.status == IDLE) {
                server_2.status = ACTIVE;
                // update_departure for the current process in the server
                departure_2.value = clk.time + (*current_process).service_time;
            } else { // both servers is active
                // update statistics
                area_under_q += (clk.time - prev_clk.time) * (int) waiting_queue.size();
                waiting_queue.push(*current_process);
                delayed_processes++;
            }
            if (current_process == processes.end() - 1) {
                next_arrival_time.value = INF;
                continue;
            }
            current_process++;
            // update_arrival to the next process
            next_arrival_time.value = (*current_process).arrival_time;
        }
    }


    /* ------- Output statistics -------- */

    cout << "The Average waiting time of those who wait in queue d(n) = "
         << total_delay_time * 1.0 / delayed_processes << " min\n";

    cout << "Time-average number in queue q(n) = " << area_under_q * 1.0 / clk.time << " process\n";

    cout << "Total busy time B(t) = " << area_under_b << " min\n";

    cout << "Utilization u(n) = " << area_under_b * 1.0 / clk.time << '\n';

    cout << "Average service time = " << area_under_b * 2.0 / double(processes.size()) << " min\n";

    cout << "Average waiting time = " << total_delay_time / double(processes.size()) << " min\n";

    cout << "Average time customer spends in the system = "
         << (total_delay_time + 2 * area_under_b) / double(processes.size()) << " min\n";
}
