'''535. Encode and Decode TinyURL
Medium

1068

2068

Add to List

Share
Note: This is a companion problem to the System Design problem: Design TinyURL.
TinyURL is a URL shortening service where you enter a URL such as https://leetcode.com/problems/design-tinyurl and it returns a short URL such as http://tinyurl.com/4e9iAk. Design a class to encode a URL and decode a tiny URL.

There is no restriction on how your encode/decode algorithm should work. You just need to ensure that a URL can be encoded to a tiny URL and the tiny URL can be decoded to the original URL.

Implement the Solution class:

Solution() Initializes the object of the system.
String encode(String longUrl) Returns a tiny URL for the given longUrl.
String decode(String shortUrl) Returns the original long URL for the given shortUrl. It is guaranteed that the given shortUrl was encoded by the same object.
 

Example 1:

Input: url = "https://leetcode.com/problems/design-tinyurl"
Output: "https://leetcode.com/problems/design-tinyurl"

Explanation:
Solution obj = new Solution();
string tiny = obj.encode(url); // returns the encoded tiny url.
string ans = obj.decode(tiny); // returns the original url after deconding it.
 

Constraints:

1 <= url.length <= 104
url is guranteed to be a valid URL.'''
"""
TinyURL is a URL shortening service where you enter a URL such as https://leetcode.com/problems/design-tinyurl and it returns a short URL such as http://tinyurl.com/4e9iAk.
Design the encode and decode methods for the TinyURL service. There is no restriction on how your encode/decode algorithm should work. You just need to ensure that a URL can be encoded to a tiny URL and the tiny URL can be decoded to the original URL.
Solution:
1. Using simple counter
We use a list to store all encountered urls in a list and the index of urls represents the encoding of urls.
Pros:
* easy to understand
Cons:
* The range of URLs that can be decoded is limited by the range of int.
* If excessively large number of URLs have to be encoded, after the range of int is exceeded, integer overflow could lead to overwriting the previous URLs' encodings, leading to the performance degradation.
* The length of the URL isn't necessarily shorter than the incoming longURL. It is only dependent on the relative order in which the URLs are encoded.
* One problem with this method is that it is very easy to predict the next code generated, since the pattern can be detected by generating a few encoded URLs.
* If I'm asked to encode the same long URL several times, it will get several entries. That wastes codes and memory.
* People can find out how many URLs have already been encoded. Not sure I want them to know.
* People might try to get special numbers by spamming me with repeated requests shortly before their desired number comes up.
* Only using digits means the codes can grow unnecessarily large. Only offers a million codes with length 6 (or smaller). Using six digits or lower or upper case letters would offer (10+26*2)6 = 56,800,235,584 codes with length 6.
2. Ramdom number + Hashtable
To solve the problems in the previous solution, we can use random number as encodings of urls.
And use 2 hashtables to record url2code and code2url.
Pros:
* Save space and memory
* Dont have to worry about spamming because the encoding of urls can not be predicted.
Cons:
* Hash collision, we have to re-generate the code if collision happens.
"""


# Using simple counter
# > 40%




import random
import string
class Codec:
    def __init__(self):
        self.map = []

    def encode(self, longUrl):
        """Encodes a URL to a shortened URL.

        :type longUrl: str
        :rtype: str
        """
        self.map.append(longUrl)
        tinyurl = 'http://tinyurl.com/{}'.format(len(self.map) - 1)
        return tinyurl

    def decode(self, shortUrl):
        """Decodes a shortened URL to its original URL.

        :type shortUrl: str
        :rtype: str
        """
        index = int(shortUrl.split('/')[-1])
        return self.map[index]


# Your Codec object will be instantiated and called as such:
# codec = Codec()
# codec.decode(codec.encode(url))


# Random encoding + hashtable
# > 20%


class Codec:
    def __init__(self):
        self.chars = string.digits + string.ascii_letters
        self.encode_len = 6
        self.url2code = {}
        self.code2url = {}

    def encode(self, longUrl):
        """Encodes a URL to a shortened URL.

        :type longUrl: str
        :rtype: str
        """
        while longUrl not in self.url2code:
            code = ''.join(random.sample(self.chars, self.encode_len))
            if code not in self.code2url:
                self.code2url[code] = longUrl
                self.url2code[longUrl] = code
            else:
                continue
        return 'http://tinyurl.com/' + code

    def decode(self, shortUrl):
        """Decodes a shortened URL to its original URL.

        :type shortUrl: str
        :rtype: str
        """
        return self.code2url[shortUrl[-6:]]
