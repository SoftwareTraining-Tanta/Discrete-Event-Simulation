#include <bits/stdc++.h>

#define ll long long
#define INF INT_MAX
#define IDLE 0
#define ACTIVE 1
using namespace std;

void init() {
//    ios_base::sync_with_stdio(false);
//    cin.tie(nullptr);
#ifndef ONLINE_JUDGE
    freopen("input.txt", "r", stdin);
#endif
}

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
    int time;
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

Clock clk, prev_clk;
vector<Process> processes;
// ----------------Statical variables
int cnt_delayed, total_delay_time;
double area_under_q, area_under_b; // (clock value - time of last event) * height
// arrival time, and service time
int process_index = 0;
bool finished;
int curr_time;

class Event {
public:
    double value;
    Event(double t) : value(t) {};
    Event() {
        value = INF;
    }
    void update_departure() {
        value = clk.time + processes[process_index].service_time;
    }
    void update_arrival() {
        value = processes[process_index].arrival_time;
    }
};

Event next_arrival_time; // A
Event current_departure_time; // D
// ----------------System state variables
Server server_1;
queue<Process> waiting_queue;
// ----------------event list



void get_time(int &curr_time) {
    if (current_departure_time.value == INF)
        curr_time = next_arrival_time.value;
    if (next_arrival_time.value < current_departure_time.value)
        curr_time = next_arrival_time.value;
    else
        curr_time = current_departure_time.value;
}

void update_clocks(double next_clk) {
    prev_clk.time = clk.time;
    clk.time = next_clk;
}

void update_graph() {
    area_under_q += (clk.time - prev_clk.time) * (int) waiting_queue.size();
    area_under_b += clk.time - prev_clk.time;
}

int main() {
    init();
    vector<double> arrival_time = {0, 5, 3, 1, 1};
    vector<double> service_time = {3, 2, 1, 2, 1};
    processes.clear();
    vector<double> arrival(arrival_time.size());
    for (int i = 1; i < arrival_time.size(); ++i)
        arrival[i] = arrival[i - 1] + arrival_time[i];
    for (int i = 0; i < arrival_time.size(); ++i) {
        processes.push_back({arrival_time[i], service_time[i], arrival[i]});
    }
//    processes.push_back({INF, INF, INF});

    next_arrival_time = processes[0].interarrival_time;
    cnt_delayed = 0, total_delay_time = 0, area_under_q = 0, area_under_b = 0, process_index = 0;
    curr_time = 0;
    int delayed_processes = 0;
    while (true) {
        get_time(curr_time);
        if (curr_time == INF)
            break;
        update_clocks(curr_time);
        if (curr_time == current_departure_time.value) { // curr_time == current_departure_time
            update_graph();
            if (!waiting_queue.empty()) { // queue is not empty
                Process curr_process = waiting_queue.front();
                waiting_queue.pop();
                total_delay_time += clk.time - curr_process.arrival_time;
                current_departure_time.value =
                        clk.time + curr_process.service_time; // add waiting time of the top process
                cnt_delayed++;
            } else { // no items to insert
                server_1.status = IDLE;
                current_departure_time.value = INF;
            }
        } else { // curr_time == arrival_time
            if (server_1.status == IDLE) { // server is idle
                server_1.status = ACTIVE;
                current_departure_time.update_departure();
                cnt_delayed++;
            } else { // server is active
                update_graph();
                waiting_queue.push(processes[process_index]);
                delayed_processes++;
            }
            if (process_index == processes.size() - 1) {
                next_arrival_time.value = INF;
                continue;
            }
            process_index++;
            next_arrival_time.update_arrival();
        }
    }

    cerr << "clock value: " << clk.time << '\n';
    cerr << "total_delay_time value: " << total_delay_time << '\n';
    cerr << "Average waiting time: " << total_delay_time / processes.size() << '\n';
    cout << "Average delay time: " << total_delay_time * 1.0 / delayed_processes << '\n';
    cout << "Average number of processes in queue: " << area_under_q * 1.0 / clk.time << '\n';
    cout << "Area under b: " << area_under_b << '\n' << "clock final value: " << clk.time << '\n';
    cout << "utilization: " << area_under_b * 1.0 / clk.time << '\n';
}
