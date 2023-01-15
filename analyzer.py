from collections import defaultdict
import itertools

class analyzer():

    def __init__(self,branchFileds_dict, lifetime_dist):
        self.branchFileds_dict = branchFileds_dict
        self.lifetime_dist = lifetime_dist

    # 返回fileds两两不重复的set组合
    def return_combination(self):
        # key_set = self.lifetime_dist.keys()

        key_set = []
        for x in self.branchFileds_dict.keys():
            key_set = list(set(self.branchFileds_dict[x]) | set(key_set))

        combination = list(itertools.combinations(key_set, 2))
        # print(conbination)
        return combination

    def lifetime_analyzer(self,field1, field2):  # 进行lifetime的分析，两个参数是两个field的lifetime取值范围集合，返回值是意义是：两个filed在时间上是否重合
        field1_start = self.lifetime_dist[field1][0]
        field1_end = self.lifetime_dist[field1][-1]
        field2_start = self.lifetime_dist[field2][0]
        field2_end = self.lifetime_dist[field2][-1]

        if (field1_start <= field2_start and field1_end >=field2_end) or (field1_start >= field2_start and field1_end <=field2_end) or (field1_start <= field2_start and field1_start >=field2_start) or (field1_start <= field2_end and field1_end >=field2_end):
            return False
        else:
            return True


    # # 需要进行分支的编号和每条分支下所含filed的读取
    # # 每个branch下出现的fileds {branch 1: [f1,f2,f3]  branch2 : [f3,f4],branch3:[f5,f6]}
    # # 转换为      每个fileds所在的branches  {f1:[1],f2:[1],f3:[1,2],f4:[2],f5:[6]}  这种形式
    def branchFields_to_filedsBranch(self):
        filedsBranch_dict = defaultdict(list)
        for key in self.branchFileds_dict.keys():
            # print(key)
            value = self.branchFileds_dict[key]
            for k in value:
                filedsBranch_dict[k].append(key)
        return filedsBranch_dict

    # 需要进行分支的编号和每条分支下所含filed的读取
    # 每个branch下出现的fileds {branch 1: [f1,f2,f3]  branch2 : [f3,f4],branch3:[f5,f6]}
    # 转换为      每个fileds所在的branches  {f1:[1],f2:[1],f3:[1,2],f4:[2],f5:[6]}  这种形式
    def exclusive_analyzer(self, field1, field2):  # 给定整体的集合和filed1,field2,求是否满足exclusive
        filedsBranch_dict = defaultdict(list)
        for key in self.branchFileds_dict.keys():
            # print(key)
            value = self.branchFileds_dict[key]
            for k in value:
                filedsBranch_dict[k].append(key)

        a = filedsBranch_dict[field1]
        b = filedsBranch_dict[field2]
        # conbination = list(itertools.combinations(filedsBranch_dict.keys(), 2))   #两两不重复组合
        # print(conbination)
        if list(set(a) & set(b))!=[]:  # 进行exclusive的分析，两个参数是：两个field各自出现的分支编号集合，如果两个filed出现的分支编号有重合
            return False
        else:
            return True
    def return_L(self):
        filedsBranch_dict = self.branchFields_to_filedsBranch()
        fields_list = list(filedsBranch_dict.keys())
        length = len(fields_list)
        L = []
        for i in range(length):
            L = L.append([1] * length)
        for i in range(length):
            for j in range(length):
                if self.lifetime_analyzer(fields_list[i],fields_list[j]):
                    L[i][j] = 0

        return L

    def return_E(self):
        filedsBranch_dict = self.branchFields_to_filedsBranch()
        fields_list = list(filedsBranch_dict.keys())
        length = len(fields_list)
        E = []
        for i in range(length):
            E = E.append([1]*length)

        for i in range(length):
            for j in range(length):
                if self.exclusive_analyzer(fields_list[i],fields_list[j]):
                    E[i][j] = 0

        return E