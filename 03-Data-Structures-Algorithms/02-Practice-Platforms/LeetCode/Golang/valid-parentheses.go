/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// Time:  O(n)
// Space: O(n)

package main

func isValid(s string) bool {
    if len(s)%2 != 0 {
        return false
    }
    stack := make([]byte, 0, len(s))
    for i := 0; i < len(s); i++ {
        c := s[i]
        switch c {
        case '(':
            stack = append(stack, ')')
        case '{':
            stack = append(stack, '}')
        case '[':
            stack = append(stack, ']')
        default:
            if len(stack) == 0 || stack[len(stack)-1] != c {
                return false
            }
            stack = stack[:len(stack)-1]
        }
    }
    return len(stack) == 0
}
