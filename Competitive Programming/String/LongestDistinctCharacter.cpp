#include<bits/stdc++.h>
using namespace std;
int main(){
	//code
    int t;
    cin >> t;
    while(t--){
        string s;
        cin >> s;
        int n = s.size();
        if(s.size()==0){ 
            return 0;
        }
        int i=0, j=0;
        vector<int> cnt(326, 0);
        cnt[s[0]]++;
        int ans=1;
        while(1){
            if(j==n-1) break;
            if(cnt[s[j+1]] == 0) j++, cnt[s[j]]++, ans=max(ans,j-i+1);
            else {
                cnt[s[i++]]--;
            }
        }
        cout << ans << endl;
    }
	return 0;
}

class Solution {
public:
    int lengthOfLongestSubstring(string s) {
        vector<int> chars(128);

        int left = 0;
        int right = 0;

        int res = 0;
        while (right < s.length()) {
            char r = s[right];
            chars[r]++;

            while (chars[r] > 1) {
                char l = s[left];
                chars[l]--;
                left++;
            }

            res = max(res, right - left + 1);

            right++;
        }

        return res;
    }
};
