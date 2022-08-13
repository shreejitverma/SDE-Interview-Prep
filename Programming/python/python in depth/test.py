import json
import urllib
import requests

url = 'https://parseapi.back4app.com/classes/Dataset_India_Pin_Code?count=1&limit=10'
headers = {
    # This is the fake app's application id
    'X-Parse-Application-Id': 'G7Z8d2KcYmU0WsiPgFIZS43FUBLvarKdx5MtgyLs',
    # This is the fake app's readonly master key
    'X-Parse-Master-Key': 'TRxCvj4TyIJFd2NBs5XyXeQxAqUpm7SageFJ2sUZ'
}
data = json.loads(requests.get(url, headers=headers).content.decode(
    'utf-8'))  # Here you have the data that you need
print(json.dumps(data, indent=2))
