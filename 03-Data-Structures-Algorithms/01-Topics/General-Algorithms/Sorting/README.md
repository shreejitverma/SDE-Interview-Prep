# Analysis of QuickSort
Analysing QuickSort over a randomly generated array.

## How?

### Data generation
I have used a random value generator(java.util.Random) class
to generate random numbers. I have taken array of different
sizes ranging from one-hundred to nine-hundred-million. In
order to maintain actual randomness and remove bias, I have 
generated a hundred-thousand arrays of the same size. 

So, one-hundred sized array is being generated a hundred-thousand
times so as to make the generation and usage of the array a bit
more random. Over those hundred-thousand arrays, I have averaged
out the time taken to sort the array using QuickSort. The QuickSort
class is written in another class in the same directory.

### QuickSort
This method is a divide-and-conquer algorithm. Pivot is blindly 
chosen to be the last element of the array, since the array is 
random so I can consider it to be balanced set of sorted, 
partially-sorted or reversely-sorted array.

## Example:
```
array = {23, 34, 45 , 56, 67, 45, 5, 56 ,3, 34, 23};
timeTaken = 1 millisecond
array = {23, 3234, 2345 , 3546, 4567, -1345, -905, 1256, 13, -34, 121323};
timeTaken = 0 millisecond
```
This generation of the same-sized array is done hundred thousand times(some of them a little less since the time was taking too long to conclude the average), 
and then the average is taken out over those values.

## A data snapshot
```
ArraySize,Time
500,0.02253
1000,0.04959
2000,0.10766
5000,0.30053
10000,0.64329
20000,1.35174
50000,3.72582
....
```

## Visualization
- [ ] To be done