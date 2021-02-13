import sys

def find_bigrams(input_list):
  return zip(input_list, input_list[1:])
  
with open('sent6.txt') as f:
    lines = f.readlines()
for line in lines:
    input_list = line.split()
    print find_bigrams(input_list)  
    

  


