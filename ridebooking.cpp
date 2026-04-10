#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

const double INF = 1e9;

struct Edge {
    int u, v;
    double w;
};

struct Driver {
    int node;
    double rating, pricePerKm;
    int type;
    bool available = true;
};

struct Rider {
    int src, dest;
};

vector<double> bellmanFord(int V, vector<Edge>& edges, int src) {
    vector<double> dist(V, INF);
    dist[src] = 0;

    for (int i = 0; i < V - 1; i++) {
        for (auto e : edges) {
            if (dist[e.u] != INF && dist[e.u] + e.w < dist[e.v]) {
                dist[e.v] = dist[e.u] + e.w;
            }
        }
    }

    for (auto e : edges) {
        if (dist[e.u] != INF && dist[e.u] + e.w < dist[e.v]) {
            cout << "Negative cycle detected!\n";
            break;
        }
    }

    return dist;
}

string vehicleName(int t) {
    string v[] = {"Bike", "Mini", "Sedan", "SUV"};
    return v[t];
}

int main() {
    int V, E;
    cout << "Enter number of nodes and edges: ";
    cin >> V >> E;

    vector<Edge> edges;

    cout << "Enter edges (u v weight):\n";
    for (int i = 0; i < E; i++) {
        int u, v;
        double w;
        cin >> u >> v >> w;

        if (u >= V || v >= V) {
            cout << "Invalid node!\n";
            i--;
            continue;
        }

        edges.push_back({u, v, w});
        edges.push_back({v, u, w});
    }

    int d, r;
    cout << "Enter number of drivers and riders: ";
    cin >> d >> r;

    vector<Driver> drivers(d);
    vector<Rider> riders(r);

    cout << "\nEnter Driver details (node rating price type[0-3]):\n";
    for (int i = 0; i < d; i++) {
        cin >> drivers[i].node >> drivers[i].rating
            >> drivers[i].pricePerKm >> drivers[i].type;

        if (drivers[i].type < 0 || drivers[i].type > 3) {
            cout << "Invalid type! Re-enter\n";
            i--;
        }
    }

    cout << "\nEnter Rider (source destination):\n";
    for (int i = 0; i < r; i++) {
        cin >> riders[i].src >> riders[i].dest;
    }

    for (int i = 0; i < r; i++) {
        cout << "\n--- Rider " << i << " ---\n";

        cout << "Vehicle Types:\n";
        cout << "0: Bike\n1: Mini\n2: Sedan\n3: SUV\n";

        int pref;
        cout << "Select vehicle type: ";
        cin >> pref;

        if (pref < 0 || pref > 3) {
            cout << "Invalid type!\n";
            continue;
        }

        vector<double> distFromRider = bellmanFord(V, edges, riders[i].src);

        struct Option {
            int id;
            double dist, fare, rating;
        };

        vector<Option> opt;

        for (int j = 0; j < d; j++) {
            if (!drivers[j].available || drivers[j].type != pref)
                continue;

            if (distFromRider[drivers[j].node] < INF) {
                double d1 = distFromRider[drivers[j].node];
                double fare = d1 * drivers[j].pricePerKm;

                opt.push_back({j, d1, fare, drivers[j].rating});
            }
        }

        if (opt.empty()) {
            cout << "No drivers available!\n";
            continue;
        }

        sort(opt.begin(), opt.end(), [](auto a, auto b) {
            if (a.fare == b.fare) {
                if (a.rating == b.rating)
                    return a.dist < b.dist;
                return a.rating > b.rating;
            }
            return a.fare < b.fare;
        });

        int limit = min(3, (int)opt.size());

        cout << "\nTop Drivers:\n";
        for (int k = 0; k < limit; k++) {
            int id = opt[k].id;
            cout << k << ". Driver " << id
                 << " (" << vehicleName(drivers[id].type) << ")"
                 << " | Distance: " << opt[k].dist
                 << " | Fare: Rs " << opt[k].fare
                 << " | Rating: " << opt[k].rating << endl;
        }

        int choice;
        cout << "Choose driver (0-" << limit - 1 << "): ";
        cin >> choice;

        if (choice < 0 || choice >= limit) {
            cout << "Invalid choice!\n";
            continue;
        }

        int sel = opt[choice].id;

        vector<double> distTrip = bellmanFord(V, edges, riders[i].src);
        double tripDist = distTrip[riders[i].dest];

        if (tripDist >= INF) {
            cout << "Destination unreachable!\n";
            continue;
        }

        double totalFare = tripDist * drivers[sel].pricePerKm;

        cout << "\nRide Started...\n";
        cout << "Trip Distance: " << tripDist << endl;
        cout << "Total Fare: Rs " << totalFare << endl;

        drivers[sel].node = riders[i].dest;

        cout << "Ride Completed! Driver " << sel << " is now available again.\n";
    }

    cout << "\nAll rides completed successfully.\n";
}
