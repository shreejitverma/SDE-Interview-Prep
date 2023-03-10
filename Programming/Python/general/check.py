
newLst = [[1,5,6],
          [2,3,4],
          [8,2,9]]

# Maximum Product Sum Path
# move right 
# move down 
maximum_sum = newLst[0][0]
# for i in range(0, len(newLst)):
#     for j in range(0, len(newLst)):
#         if :
i=j=0
while i<len(newLst) and j<len(newLst):
    if i == len(newLst)-1:
        j+=1
        maximum_sum += newLst[i][j+1]
        continue
    if j == len(newLst) - 1:
        i+=1
        maximum_sum += newLst[i+1][j]
        continue
    if newLst[i][j+1]>= newLst[i+1][j]:
        j+=1
        maximum_sum+= newLst[i][j+1]
    else:
        i+=1
        maximum_sum += newLst[i+1][j]
        
print(maximum_sum)
 
