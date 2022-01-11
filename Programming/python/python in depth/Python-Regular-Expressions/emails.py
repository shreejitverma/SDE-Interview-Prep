import re

emails = '''
ShreejitMVerma@gmail.com
shreejit.verma@university.edu
shreejit-321-verma@my-work.net
'''

pattern = re.compile(r'[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\.[a-zA-Z0-9-.]+')

matches = pattern.finditer(emails)

for match in matches:
    print(match)
