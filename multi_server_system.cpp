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

class process {
public:
    int arrival_time;
    int service_time;
    process(int arrival, int service) : arrival_time(arrival), service_time(service) {};
    process() {
        arrival_time = 0;
        service_time = 0;
    }
};

int main() {
    init();
    // ----------------Initializing time
    int time = 0;
    // ----------------System state variables
    bool server_status1 = IDLE; // 1 if busy, 0 if idle
    bool server_status2 = IDLE; // 1 if busy, 0 if idle

    int previous_clock = 0;
    queue<process> waiting_queue;
    // ----------------event list
    int clock = 0;
    int next_arrival_time = 0; // A
    int current_departure_time_1 = INF; // B
    int current_departure_time_2 = INF; // C
    // ----------------Statical variables
    int cnt_delayed = 0;
    int total_delay_time = 0;
    int area_under_q = 0;
    int area_under_b = 0; // (clock value - time of last event) * height
    // arrival time, and service time
    int process_index = 0;
    vector<int> arrival_time = {0, 2, 4, 4, 2,2};
    vector<int> service_time = {5, 3, 3, 5, 6,3};
    vector<process> processes;
    for (int i = 0; i < arrival_time.size(); ++i) {
        processes.push_back({arrival_time[i], service_time[i]});
    }

    bool finished = false;
    next_arrival_time = processes[0].arrival_time; // 0
    int curr_time = 0;
    while (!finished) {
       
        curr_time = min(next_arrival_time, min(current_departure_time_1, current_departure_time_2));

        if (curr_time == INF)
            break;

        if (curr_time == next_arrival_time) {
            previous_clock = clock;
            clock = next_arrival_time;
            if (server_status1 == IDLE) { // server is idle
                current_departure_time_1 = clock + processes[process_index].service_time;
                if (process_index == processes.size() - 1) {
                    next_arrival_time = INF;
                    continue;
                } else
                    process_index++;
                next_arrival_time += processes[process_index].arrival_time;
                cnt_delayed++;
                server_status1 = ACTIVE;
            } 
            else if(server_status2==IDLE){
                current_departure_time_2 = clock + processes[process_index].service_time;
                if (process_index == processes.size() - 1) {
                    next_arrival_time = INF;
                    continue;
                } else
                    process_index++;
                next_arrival_time += processes[process_index].arrival_time;
                cnt_delayed++;
                server_status2 = ACTIVE;
            }
            else { // both servers are active
                area_under_q += (clock - previous_clock) * (int) waiting_queue.size();
                area_under_b += clock - previous_clock;
                waiting_queue.push(processes[process_index]);
                if (process_index == processes.size() - 1) {
                    next_arrival_time = INF;
                    continue;
                } else
                    process_index++;
            }
        } else if(curr_time==current_departure_time_1){ // curr_time == current_departure_time
            previous_clock = clock;
            clock = current_departure_time_1;
            if (!waiting_queue.empty()) {
                area_under_q += (clock - previous_clock) * (int) waiting_queue.size();
                area_under_b += clock - previous_clock;
                process curr_process = waiting_queue.front();
                total_delay_time += clock - curr_process.arrival_time;
                current_departure_time_1 += curr_process.service_time; // add waiting time of the top process
                waiting_queue.pop();
                cnt_delayed++;
            } else {
                server_status1 = IDLE;
                area_under_b += clock - previous_clock;
                current_departure_time_1 = INF;
            }
        }
        else {
            previous_clock = clock;
            clock = current_departure_time_2;
            if (!waiting_queue.empty()) {
                area_under_q += (clock - previous_clock) * (int) waiting_queue.size();
                area_under_b += clock - previous_clock;
                process curr_process = waiting_queue.front();
                total_delay_time += clock - curr_process.arrival_time;
                current_departure_time_2 += curr_process.service_time; // add waiting time of the top process
                waiting_queue.pop();
                cnt_delayed++;
            } else {
                server_status2 = IDLE;
                area_under_b += clock - previous_clock;
                current_departure_time_2 = INF;
            }
        }
    }

    cout << "Average delay time: " << total_delay_time * 1.0 / cnt_delayed << '\n';
    cout << "Average number of processes in queue: " << area_under_q * 1.0 / clock << '\n';
    cout << "utilization: " << area_under_b * 1.0 / clock << '\n';
}
