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
    double arrival_time;
    double service_time;
    double arrival;
    Process(double arrival, double service, double arr) : arrival_time(arrival), service_time(service), arrival(arr) {};
    Process() {
        arrival_time = 0;
        service_time = 0;
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
int cnt_delayed;
double total_delay_time;
double area_under_q, area_under_b; // (clock value - time of last event) * height
// arrival time, and service time
int process_index;
bool finished;
double curr_event;

class Event {
public:
    double value;
    int number = 0;
    Event(double t, int n) : value(t), number(n) {};
    Event() {
        value = INF;
        number = 0;
    }
    Event(int n) : number(n) {};
    void update_departure() {
        value = clk.time + processes[process_index].service_time;
    }
    void update_arrival() {
        value += processes[process_index].arrival_time;
    }
};

Event next_arrival_time(0); // A
Event current_departure_time_1(1), current_departure_time_2(2); // D1, D2
// ----------------System state variables
Server Michael, Badea;
queue<Process> waiting_queue;
// ----------------event list



int get_time() {
    double curr_time = min(
            {
                    next_arrival_time.value,
                    current_departure_time_1.value,
                    current_departure_time_2.value
            });
    if (curr_time == INF)
        return INF;
    if (curr_time == next_arrival_time.value)
        return 0;
    else if (curr_time == current_departure_time_1.value)
        return 1;
    else if (curr_time == current_departure_time_2.value)
        return 2;
    else
        return INF;
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
    vector<double> arrival_time = {0, 2, 4, 4, 2, 2};
    vector<double> service_time = {5, 3, 3, 5, 6, 3};
    processes.clear();
    vector<double> arrival(arrival_time.size());
    for (int i = 1; i < arrival_time.size(); ++i)
        arrival[i] = arrival[i - 1] + arrival_time[i];
    for (int i = 0; i < arrival_time.size(); ++i) {
        processes.push_back({arrival_time[i], service_time[i], arrival[i]});
    }
    next_arrival_time = processes[0].arrival_time;
    cnt_delayed = 0, total_delay_time = 0, area_under_q = 0, area_under_b = 0, process_index = 0;
    finished = false;
    curr_event = 0;
    int delayed_processes = 0;
    while (!finished) {
        int curr_event = get_time();
        if (curr_event == INF)
            break;
        if (curr_event == 0) {
            update_clocks(next_arrival_time.value);
            if (Michael.status == IDLE) { // server is idle
                current_departure_time_1.update_departure();
                if (process_index == processes.size() - 1) {
                    next_arrival_time = INF;
                    continue;
                } else
                    process_index++;
                next_arrival_time.update_arrival();
                cnt_delayed++;
                Michael.status = ACTIVE;
            } else if (Badea.status == IDLE) { // server is idle
                current_departure_time_2.update_departure();
                if (process_index == processes.size() - 1) {
                    next_arrival_time.value = INF;
                    continue;
                } else
                    process_index++;
                next_arrival_time.update_arrival();
                cnt_delayed++;
                Badea.status = ACTIVE;
            } else { // both servers are active
                update_graph();
                waiting_queue.push(processes[process_index]);
                delayed_processes++;
                if (process_index == processes.size() - 1) {
                    next_arrival_time = INF;
                    continue;
                } else
                    process_index++;
            }
        } else if (curr_event == 1) { // curr_time == current_departure_time_1
            update_clocks(current_departure_time_1.value);
            if (!waiting_queue.empty()) { // queue is not empty
                update_graph();
                Process curr_process = waiting_queue.front();
                waiting_queue.pop();
                total_delay_time += clk.time - curr_process.arrival;
                current_departure_time_1.value += curr_process.service_time; // add waiting time of the top process
                cnt_delayed++;
            } else { // no items to insert
                Michael.status = IDLE;
                update_graph();
                current_departure_time_1.value = INF;
            }
        } else if (curr_event == 2) { // curr_time == current_departure_time_2
            update_clocks(current_departure_time_2.value);
            if (!waiting_queue.empty()) { // queue is not empty
                update_graph();
                Process curr_process = waiting_queue.front();
                waiting_queue.pop();
                total_delay_time += clk.time - curr_process.arrival;
                current_departure_time_2.value += curr_process.service_time; // add waiting time of the top process
                cnt_delayed++;
            } else { // no items to insert
                Badea.status = IDLE;
                update_graph();
                current_departure_time_2.value = INF;
            }
        }
    }

    cerr << "clock value: " << clk.time << '\n';
    cerr << "total_delay_time value: " << total_delay_time << '\n';
    cout << "Average delay time: " << total_delay_time * 1.0 / delayed_processes << '\n';
    cout << "Average number of processes in queue: " << area_under_q * 1.0 / clk.time << '\n';
    cout << "Area under b: " << area_under_b << '\n' << "clock final value: " << clk.time << '\n';
    cout << "utilization: " << area_under_b * 1.0 / clk.time << '\n';
}
