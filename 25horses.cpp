#include <algorithm>
#include <array>
#include <iostream>
#include <map>
#include <numeric>
#include <vector>
using namespace std;

class Horse {
public:
  Horse() {}
  Horse(int id_, int speed_) : id(id_), speed(speed_) {
  } 
  int GetID() const {
    return id;
  }
  friend class Court;
private:
  int id; // id从0开始数
  int speed;
};

class Court {
public:
  Court() : times(0) {
  }
  array<int, 5> Race(const array<Horse, 5> &horses) {
    // 返回值：最前面为第一名的id
    ++times;
    array<int, 5> ranks;
    iota(ranks.begin(), ranks.end(), 0);
    sort(ranks.begin(), ranks.end(), [&horses](int a, int b) {
        return horses[a].speed > horses[b].speed;
    });
    array<int, 5> ids;
    for (int i = 0; i < 5; ++i) {
      ids[i] = horses[ranks[i]].GetID();
    }
    return ids;
  }
  inline int GetTimes() const {
    return times;
  }
  void Check(const vector<Horse> &horses, array<int, 3> predRank3s) {
    // predRank3s是ID号而不是数组下标
    bool fail = false;
    if (times > 7) {
      cout << "比赛次数大于7, 方法不正确！" << endl;
      fail = true;
    }
    vector<int> gtRanks = GetRanks(horses);
    array<int, 3> gtRank3s;
    for (int i = 0; i < 3; ++i) {
      gtRank3s[i] = gtRanks[i];
    }
    if (gtRank3s != predRank3s) {
      cout << "计算的排名错误！" << endl;
      cout << "GT: ";
      for (int t : gtRank3s) cout << t << ", ";
      cout << endl;

      cout << "PRED: ";
      for (int t : predRank3s) cout << t << ", ";
      cout << endl;
      fail = true;
    }
    if (fail)
      cout << "计算错误，比赛次数：" << times << endl;
    else
      cout << "计算正确，比赛次数：" << times << endl;
  }
private:
  vector<int> GetRanks(const vector<Horse> &horses) {
    vector<int> ranks(horses.size());
    iota(ranks.begin(), ranks.end(), 0);
    sort(ranks.begin(), ranks.end(), [&horses](int a, int b) {
        return horses[a].speed > horses[b].speed;
    });
    vector<int> ids(horses.size());
    for (int i = 0; i < horses.size(); ++i) {
      ids[i] = horses[ranks[i]].GetID();
    }
    return ids;
  }
private:
  int times;
};

vector<Horse> GetHorses() {
  const int N = 25;
  vector<int> speeds(N);
  iota(speeds.begin(), speeds.end(), 0);
  random_shuffle(speeds.begin(), speeds.end());
  vector<Horse> out(N);
  for (int i = 0; i < N; ++i) {
    out[i] = Horse(i, speeds[i]);
  } 
  return out;
}

array<int, 3> Strategy(const vector<Horse> &horses, Court& court) {
  unordered_map<int, Horse> id2horse;
  for (int i = 0; i < horses.size(); ++i) {
    id2horse[horses[i].GetID()] = horses[i];
  }
  // 首先平均分为5组进行比赛, 共5次
  vector<array<int, 5>> groupRanks(5);
  for (int g = 0; g < 5; ++g) {
    array<Horse, 5> group;
    for (int i = 0; i < 5; ++i) {
      group[i] = horses[g * 5 + i];
    }
    groupRanks[g] = court.Race(group);
  }
  // 将每组第一名的马选出来，进行比赛, 共1次
  array<Horse, 5> groupWinners;
  for (int g = 0; g < 5; ++g) {
    groupWinners[g] = id2horse[groupRanks[g][0]]; 
  }
  array<int, 5> groupWinnersRank = court.Race(groupWinners);
  // 找到前三名所在的组的编号
  array<int, 3> top3groups;
  for (int i = 0; i < 3; ++i) {
    int hid = groupWinnersRank[i];
    for (int g = 0; g < 5; ++g) {
      if (groupRanks[g][0] == hid) {
        top3groups[i] = g;
        break;
      }
    }
  }
  // groupWinners中第一名所在的组的第二、第三名
  // groupWinners第二名与其所在的组的第二名 
  // groupWinners的第三名
  array<int, 5> finalHorsesIds = {groupRanks[top3groups[0]][1],
                                  groupRanks[top3groups[0]][2],
                                  groupRanks[top3groups[1]][0],
                                  groupRanks[top3groups[1]][1],
                                  groupRanks[top3groups[2]][0]};
  array<Horse, 5> finalHorses;
  for (int i = 0; i < 5; ++i) {
    finalHorses[i] = id2horse[finalHorsesIds[i]];
  }
  // 最后一次比赛
  array<int, 5> finalRank = court.Race(finalHorses);
  return {groupWinnersRank[0], finalRank[0], finalRank[1]};
}

int main() {
  vector<Horse> horses = GetHorses();
  Court court;
  array<int, 3> predRank3s = Strategy(horses, court);
  court.Check(horses, predRank3s);
  return 0;
}
