import heapq
import json
from heapq import nlargest
f = open('/Users/shreejitverma/Documents/GitHub/Competitive Programming/Programming/python/python in depth/Python-JSON/marks.json')
data = json.load(f)
student_id = []
for i in data['marks']:
    if i['id'] not in student_id:
        student_id.append(i['id'])
rank_dict = {}
for j in student_id:
    total_marks = 0
    sid = j
    # print(sid)
    for i in data['marks']:
        if (i['id'] == sid):
            if(i['subject'] == 'Math'):
                total_marks += i['marks']
            if(i['subject'] == 'Phy'):
                total_marks += i['marks']
            if(i['subject'] == 'Chem'):
                total_marks += i['marks']
    rank_dict[sid] = total_marks

def FindNthRank(N, rank_dict, data):
    res = nlargest(N, rank_dict, key=rank_dict.get)
    # print(res)
    names = []
    for i in data['marks']:
        sid=i['id']
        name=i['name']
        if sid in res:
            if name not in names:
                names.append(name)
    print(names)

FindNthRank(2, rank_dict, data)
f.close()
