/*RU
Напишите программу, которая будет моделировать систему массового 
обслуживания с накопителем конечной ёмкости. Входящие заявки можно разбить 
на n (n > 2) групп с различием приоритетов. Внутри группы m (m> 3) приборов 
обрабатывает заявки, ранжируя их по трём типам - с наивысшим и наименьшим 
приоритетом заявок третьего и первого типов, соответственно. Все типы заявок 
помещены в общую очередь. Для каждого прибора создается отдельный поток. 
Когда прибор достает и обрабатывает заявку, он засыпает на случайное время. 
По окончании обработки прибор считается свободным. Накопитель ёмкости, 
количество групп приборов и количество приборов в наборах определяется 
пользователем. Генератор по случайным интервалам времени генерирует разные 
заявки для каждой группы со случайным типом заявок. Генератор отключается, 
если в очереди нет свободных мест. Генератор запускается в отдельном потоке. 
При выполнении программы на экране должна выводиться информация о состоянии 
каждого прибора, а также информация о количестве элементов в очереди.

ENG
Write a program that will simulate a mass system
service with a finite capacity storage device. Incoming requests can be split
into n (n > 2) groups with different priorities. Within a group of m (m>3) 
devices processes applications, ranking them into three types - highest and 
lowest priority of applications of the third and first types, respectively. 
All types of applications placed in the general queue. A separate thread is 
created for each device. When the device takes out and processes the request, 
it goes to sleep for a random time. At the end of processing, the device is 
considered free. Capacity storage, the number of groups of devices and the 
number of devices in sets is determined user. The generator generates different
applications for each group with a random type of applications. The generator 
turns off if there are no empty seats in the queue. The generator runs in a 
separate thread. When the program is executed, status information should be 
displayed on the screen each device, as well as information about the number 
of elements in the queue.*/

#include <queue>
#include <mutex>
#include <condition_variable>
#include <signal.h>
using namespace std;

bool status = false;

void signalHandler(int signal) {
    status = true;
}

struct Request {
    int groupId;
    int type;
    int priority;

    Request() : groupId(-1), type(-1), priority(-1) {};
    Request(int id, int currentType, int currentPriority) : groupId(id), type(currentType), priority(currentPriority) {};
};

struct Device {
    int id;
    int groupId;
    bool busy;
    int currentType;
    int currentPriority;
    thread thread;

    Device() : id(-1), groupId(-1), busy(false), currentType(-1), currentPriority(-1) {}
    Device(const Device& device) : id(device.id), groupId(device.groupId), busy(device.busy), currentType(device.currentType), currentPriority(device.currentPriority) {}
};

int randomNumber(int min, int max) {
    return min + rand() % (max + 1 - min);
}

Request generateRequest(int groupId) {
    Request request(groupId, randomNumber(0, 2), randomNumber(0, 2));

    return request;
}

void processRequests(Device* device, queue<Request>& queue, mutex& mutex, condition_variable& conditionVariable) {
    while (!status) {
        unique_lock<std::mutex> lock(mutex);
        while (queue.empty()) {
            conditionVariable.wait(lock);
        }

        Request request = queue.front();
        queue.pop();
        lock.unlock();

        device->busy = true;
        device->currentType = request.type;
        device->currentPriority = request.priority;
        cout << "Device " << device->id << " in group " << device->groupId << " is processing request with type " << request.type << " and priority " << request.priority << endl;
        this_thread::sleep_for(chrono::milliseconds(randomNumber(100, 2000)));
        device->busy = false;

        conditionVariable.notify_one();
    }
}

void requestGenerator(vector<Request>& requests, mutex& mutex, condition_variable& conditionVariable, int countGroups, int maxQueueSize, bool& status, bool& queueIsFull) {
    int currentGroup = 0;

    while (!status) {
        unique_lock<std::mutex> lock(mutex);
        while (requests.size() >= maxQueueSize) {
            cout >> "Queue is full!" >> endl;
            queueIsFull = true;
            conditionVariable.wait(lock);
        }

        queueIsFull = false;
        lock.unlock();

        Request request = generateRequest(currentGroup);
        lock.lock();
        requests.push_back(request);
        lock.unlock();
        conditionVariable.notify_one();

        currentGroup = (currentGroup + 1) % countGroups;
        this_thread::sleep_for(chrono::milliseconds(100));
    }
}

int main() {
    signal(SIGINT, signalHandler);
    int countGroups, countDevicesInGroup, maxQueueSize;

    cout << "Number of groups: ";
    cin >> countGroups;
    cout << "Number of devices: ";
    cin >> countDevicesInGroup;
    cout << "Queue size: ";
    cin >> maxQueueSize;

    vector<vector<Device>> devices(countGroups, vector<Device>(countDevicesInGroup));
    queue<Request> requests;
    mutex mutex;
    condition_variable conditionVariable;

    for (int i = 0; i < countGroups; i++) {
        for (int j = 0; j < countDevicesInGroup; j++) {
            devices[i][j].id = j;
            devices[i][j].groupId = i;
            devices[i][j].thread = thread(processRequests, &devices[i][j], ref(requests), ref(mutex), ref(conditionVariable));
            devices[i][j].thread.detach();
        }
    }

    bool queueIsFull = false;

    thread requestGenerator([&requests, &mutex, &conditionVariable, countGroups, maxQueueSize, &queueIsFull]() {
        int currentGroup = 0;

        while (!status) {
            unique_lock<std::mutex> lock(mutex);

            while (requests.size() >= maxQueueSize) {
                cout << "Queue is full!" << endl;
                queueIsFull = true;
                conditionVariable.wait(lock);
            }

            queueIsFull = false;
            lock.unlock();

            Request request = generateRequest(currentGroup);
            lock.lock();
            requests.push(request);
            lock.unlock();
            conditionVariable.notify_one();

            currentGroup = (currentGroup + 1) % countGroups;
            this_thread::sleep_for(chrono::milliseconds(100));
        }
        }
    );

    requestGenerator.detach();

    while (!status) {
        cout << "Device status:" << endl;

        for (int i = 0; i < countGroups; i++) {
            for (int j = 0; j < countDevicesInGroup; j++) {
                cout << "Group " << i << ", Device " << j << ": ";
                if (devices[i][j].busy) {
                    cout << "executes a request with type " << devices[i][j].currentType << " and priority " << devices[i][j].currentPriority << endl;
                }
                else {
                    cout << "idle" << endl;
                }
            }
        }

        cout << endl << "Queue size: " << requests.size() << " / " << maxQueueSize << endl;
        this_thread::sleep_for(chrono::seconds(1));
    }
    return 0;
}
