from .parser import run_parser
from .analyzer import analyzer
from .optimizer import melody
from gurobipy import *
import time
import graphviz
import os
import pandas as pd
import json
from collections import defaultdict
import itertools

dot_file_dir = '.dot'
json_file_dir = '.json'

branchFileds_dict,lifetime_dist = run_parser(dot_file_dir,json_file_dir)
field_cnt, field_width, word_cnt, word_width = len(list(lifetime_dist.keys())),1,1,1    #

define_analyzer = analyzer(branchFileds_dict,lifetime_dist)
L = define_analyzer.return_L()
E = define_analyzer.return_E()
return_object_X, return_time = melody(field_cnt, field_width, word_cnt, word_width, L, E)

print(return_object_X)
print(return_time)
