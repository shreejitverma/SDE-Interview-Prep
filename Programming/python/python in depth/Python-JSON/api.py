import json
# from urllib.request import urlopen

# with urlopen("https://finance.yahoo.com/webservice/v1/symbols/allcurrencies/quote?format=json") as response:
#     source = response.read()

# data = json.loads(source)

# # print(json.dumps(data, indent=2))

# usd_rates = dict()

# for item in data['list']['resources']:
#     name = item['resource']['fields']['name']
#     price = item['resource']['fields']['price']
#     usd_rates[name] = price

# print(50 * float(usd_rates['USD/INR']))

# b = json.dumps()
# student = {}
# student["a"] = {"rollno": 2001, "age": 18, "mark": [85, 65, 65]}
# student["b"] = {"rollno": 2002, "age": 18, "mark": [85, 55, 65]}
# student["c"] = {"rollno": 2003, "age": 18, "mark": [85, 95, 65]}
# rank = {}
# for key, value in student.items():
#     smark = sum(student[key]["mark"])
#     student[key].update({"total_marks": smark})
# s = sorted(student, key=lambda x: student[x]["total_marks"], reverse=True)
# rank = 1
# for key in s:
#     student[key].update({"rank": rank})
#     rank += 1
# print("*** Rank of each student based on total marks *** \n")
# for i in student.items():
#     print(i)


