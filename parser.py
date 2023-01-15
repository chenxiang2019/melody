import graphviz
import os
import pandas as pd
import json

# for i in os.listdir('out'):
# dot_content = graphviz.Source.from_file('out2/ingress.dot')
def read_files(dot_file,json_file):
    dot_content = graphviz.Source.from_file(dot_file)
    dot_content = str(dot_content)
    with open(json_file, 'r') as load_f:
        json_info = json.load(load_f)
    return dot_content,json_info



def parse_dot_file(dot_content):
    # print(type(dot_content))

    # print(dot_content)
    #以数字开头的元素
    list_dir =dot_content.split('\n')[0:-1]
    select = [x for x in list_dir if x[0].isdigit()]
    #有两类，一类是0，1，2，3状态， 一类是转移的边
    node = [x for x in select if x[1]=='[' or x[2]=='[' or x[3] =='[']

    ellipse_nodeID = []   #找出椭圆形的节点
    ellipse_node = []
    ellipse_node_label = []   #node_label是nodeID对应的label


    #找出椭圆形的node,shape
    for index,x in enumerate(node):
        for i in range(len(x)-2,2,-1):
            if x[i] ==',':
                for j in range(i-1,1,-1):
                    if x[j] =='e' and x[j-2] =='a' and x[j-4] =='s':
                        if x[j+2:i] == 'ellipse':
                            ellipse_nodeID.append(index)
                            ellipse_node.append(x)
                            break
    #找到nodeID的label
    for index,x in enumerate(ellipse_node):
        start,end = 0,0
        for i in range(7,len(x)):
            if x[i-2] == 'e' and x[i-1] =='l' and x[i] == '=':
                if x[i+1] =='"':
                    start = i+2
                else:
                    start = i+1
                for j in range(start,len(x)):
                    if x[j] !=',' and x[j+1] ==',':
                        if x[j] != '"':
                            end = j+1
                        else:
                            end = j
                        ellipse_node_label.append(x[start:end])
                        break
    ellipse_nodeID_label_dict = dict(zip(ellipse_nodeID,ellipse_node_label))
    # print('================ellipse=============================')
    # print(ellipse_node)
    # print(ellipse_nodeID)
    # print(ellipse_node_label)
    # print(ellipse_nodeID_label_dict)
    # print('================ellipse=============================')
    #找出长方形的node,shape
    rectangle_nodeID = []
    rectangle_node =  []
    rectangle_node_label = []
    for index,x in enumerate(node):
        for i in range(len(x)-2,2,-1):
            if x[i] ==',':
                for j in range(i-1,1,-1):
                    if x[j] =='e' and x[j-2] =='a' and x[j-4] =='s':
                        if x[j+2:i] == 'rectangle':
                            rectangle_nodeID.append(index)
                            rectangle_node.append(x)
                            break

    for index,x in enumerate(rectangle_node):
        start,end = 0,0
        for i in range(7,len(x)):
            if x[i-2] == 'e' and x[i-1] =='l' and x[i] == '=':
                if x[i+1] =='"':
                    start = i+2
                else:
                    start = i+1
                for j in range(start,len(x)):
                    if x[j] !=',' and x[j+1] ==',':
                        if x[j] != '"':
                            end = j+1
                        else:
                            end = j
                        rectangle_node_label.append(x[start:end])
                        break
    rectangle_nodeID_label_dict = dict(zip(rectangle_nodeID,rectangle_node_label))
    # print('================rectangle=============================')
    # print(rectangle_node)
    # print(rectangle_nodeID)
    # print(rectangle_node_label)
    # print(rectangle_nodeID_label_dict)
    # print('================rectangle=============================')
    edge = [x for x in select if x[1] ==' ' or x[2] ==' ' or x[3] == ' ']
    nodeID_of_edge = []
    for x in edge:
        start_index,end_index,starID,endID = 0,0,0,0
        for i in range(2,len(x)):
            if x[i] =='-' and x[i+1]=='>':
                starID = int(x[0:i-1])
                for j in range(i+2,len(x)):
                    if x[j] =='[':
                        endID = int(x[i+2:j])
                        nodeID_of_edge.append([starID,endID])
                        break

    # print('nodeID_of_edge'+str(nodeID_of_edge))


    edge_label = []
    for x in edge:
        for i in range(2,len(x)):
            if x[i] == '=':
                for j in range(i+1,len(x)):
                    if x[j] ==']':
                        if(x[j-1]=='"'):
                            edge_label.append(x[i+2:j-1])
                        else:
                            edge_label.append(x[i+1:j])
                        break
    # print('---------------')
    # print('edge_label:'+str(edge_label))
    # print('---------------')

    for x in edge:
        start_index,end_index,starID,endID = 0,0,0,0
        for i in range(2,len(x)):
            if x[i] =='-' and x[i+1]=='>':
                starID = int(x[0:i-1])
                for j in range(i+2,len(x)):
                    if x[j] =='[':
                        endID = int(x[i+2:j])
                        nodeID_of_edge.append([starID,endID])
                        break
    # edge_label_dict = dict(zip(nodeID_of_edge,edge_label))
    # print(edge_label_dict)
    # print(node)

    # print(edge)

    #1.根据边和点抽象出stage关系和if-else分支关系:1. 每个stage中的table,每个table中的field, 2.每个分支中的table,每个分支中的field ,并返回特定格式
    #2.从table找到对应的fileds,确定fields lifetime  && 找到fileds的所在分支
    #table找fileds可以从json入手
    #运行得到实例：p4c-bm2-ss --p4v 16 --p4runtime-files build/basic.p4.p4info.txt -o build/basic.json basic.p4
    #3.将不同dot文件不同control的结果合并，去除依赖，去重

    field = []
    rectangle = []

    return nodeID_of_edge,edge_label,ellipse_nodeID,ellipse_nodeID_label_dict,ellipse_node_label,rectangle_nodeID,rectangle_node_label,rectangle_nodeID_label_dict
#------------------------------------------------------------------------------------
#----------------------------------table和field的关系--------------------------------------------------
#------------------------------------------------------------------------------------


def parse_json(json_info):

    # print(json_info)
    pipelines = json_info['pipelines']
    print(pipelines)
    # print(len(pipelines))
    control_block_name_list = []
    table_name_list = []
    field_name_list = []
    controlBlock_table_filed_dict = dict()                      #形成一个三维数据结构
    # controlBlock_table_dict = ()
    for i in pipelines:
        controlBlock_name = i['source_info']['source_fragment']
        tmp_table_filed_dict = dict()
        for table_item in i['tables']:
            table_name_list.append(table_item['name'])
            field_name = []
            for key_item in table_item['key']:
                field_name.append(key_item['name'])
                field_name_list.append(key_item['name'])
            tmp_table_filed_dict[table_item['name']] = field_name
        # control_block_name_list.append(controlBlock_name)
        controlBlock_table_filed_dict[controlBlock_name] = tmp_table_filed_dict
    # print(controlBlock_table_filed_dict)
    # print(table_name_list)
    # print(field_name_list)
    return controlBlock_table_filed_dict
#-----------------------------------------------------------------------------------------------------------------
#------------------------------------------------table和stage之间的关系（时间上的），应该从dot文件中去找-----------------------------
#----------------------------------------------找到属于同一stage的nodeID(可以从中过滤出table)-------------------------------------------------------------------
#---------------------------------------------------------------------图论的内容,边，深度------------------

#检测并去除依赖
def has_depedency(index1, index2, nodeID_of_edge):
    stack = []

    for item_edge in nodeID_of_edge:
        if item_edge[1] == index2:
            return True
        elif item_edge[0] == index1:
            stack.append((item_edge[1]))

    while len(stack) != 0:
        for i in stack:
            for item_edge in nodeID_of_edge:
                if item_edge[1] == index2:
                    return True
                elif i == item_edge[0]:
                    stack.append(item_edge[1])
            stack.remove(i)
    return False





def return_dict(nodeID_of_edge,edge_label,ellipse_nodeID,ellipse_node_label,rectangle_nodeID,rectangle_node_label,ellipse_nodeID_label_dict,rectangle_nodeID_label_dict,controlBlock_table_filed_dict):
    #子问题：由nodeID_of_edge求在同一深度上的所有节点ID，返回列表形式
    # nodeID_of_edge
    hash_nodeID_list = [0]*2*len(nodeID_of_edge)     #节点ID的哈希散列表，用于计算节点值的范围和实际个数

    for i in nodeID_of_edge:
        hash_nodeID_list[i[0]] = 1
        hash_nodeID_list[i[1]] = 1
    len_range_min,len_range_max = 1,len(nodeID_of_edge)
    for index,i in enumerate(hash_nodeID_list):
        if i == 1:
            continue
        else:
            len_range_max = index
            break
    #接下来计算每个节点的出度和入度
    in_vertex =  [0]*(len_range_max)
    out_vertex = [0]*(len_range_max)
    for i in nodeID_of_edge:
        in_vertex[i[1]]+=1
        out_vertex[i[0]]+=1

    #求DAG的深度以及每一层的节点ID

    start = nodeID_of_edge[0][0]
    queue = []
    nodeid_of_each_stage = []
    end_index = 0
    tmp_len = 0
    last_len = 0
    while 1:

        for index, item in enumerate(in_vertex):
            if item == 0 and (index not in queue):
                queue.append(index)
                tmp_len += 1
                end_index+=1
        if tmp_len == 0:
            break
        nodeid_of_each_stage.append(queue[end_index-tmp_len:end_index])

        for pair in nodeID_of_edge:
            if pair[0] in queue[end_index-tmp_len:end_index]:
                in_vertex[pair[1]] -= 1
        # last_len = tmp_len
        tmp_len = 0

    # print(nodeid_of_each_stage)
    tableID_label = dict(zip(ellipse_nodeID,ellipse_node_label))
    # print(tableID_label)
    #-------------------------------接下来看if-else分支（也就是TRUE FALSE分支）中包含的table---------------------

    branches_tables = []
    def recursion_find_branch_tables(nodeid):
        if nodeid == 1:
            return
        elif nodeid in rectangle_nodeID:
            for index,tmp_edge in enumerate(nodeID_of_edge):
                if nodeid == tmp_edge[0]:
                    end_node = tmp_edge[1]
                    label = edge_label[index]
                    # print(index)
                    if end_node == 1:
                        return
                    elif end_node in ellipse_nodeID and label == 'TRUE':                 #如果终止节点不是判断方框
                        branches_tables.append([end_node])
                        recursion_find_branch_tables(end_node)
                    elif end_node in ellipse_nodeID and label == 'FALSE':
                        branches_tables.append([end_node])
                        recursion_find_branch_tables(end_node)
                    elif end_node in rectangle_nodeID and label =='FALSE':
                        recursion_find_branch_tables(end_node)
                    elif end_node in ellipse_nodeID and label == '':
                        pass
                elif nodeid < tmp_edge[0]:
                    break
        elif nodeid in ellipse_nodeID:
            for index, tmp_edge in enumerate(nodeID_of_edge):
                if nodeid == tmp_edge[0]:
                    end_node = tmp_edge[1]
                    label = edge_label[index]
                    if end_node == 1:
                        return
                    if end_node in rectangle_nodeID and label == '':
                        for index,tables in enumerate(branches_tables):
                            if tmp_edge[0] in tables and tmp_edge[0] in ellipse_nodeID:
                                # branches_tables[index].append(end_node)
                                recursion_find_branch_tables(end_node)
                    elif end_node in ellipse_nodeID and label == '':
                        for index,tables in enumerate(branches_tables):
                            if tmp_edge[0] in tables and tmp_edge[0] in ellipse_nodeID:
                                branches_tables[index].append(end_node)
                                recursion_find_branch_tables(end_node)
                elif nodeid < tmp_edge[0]:
                    break

    start_nodeid = 0
    for i in nodeID_of_edge:
        if i[0] in rectangle_nodeID and rectangle_nodeID_label_dict[i[0]]!='__START__':
            start_nodeid = i[0]

    recursion_find_branch_tables(start_nodeid)
    new_branches_tables = []
    print(branches_tables)
    print('----------------------------------------------------')
    for item1 in branches_tables:
        new_branches_tables_item = []
        for item2 in item1:
            name = ellipse_nodeID_label_dict[item2]
            new_branches_tables_item.append(name)
        new_branches_tables.append(new_branches_tables_item)
    #进行去重
    pop_index_list=[]
    for i in range(0,len(new_branches_tables)):
        for j in range(i+1,len(new_branches_tables)):
            if new_branches_tables[i] == new_branches_tables[j]:
                if j not in pop_index_list:
                    pop_index_list.append(j)
    print(pop_index_list)
    branches_tables_after_handle = []
    for i in range(len(branches_tables)):
        if i not in pop_index_list:
            branches_tables_after_handle.append(branches_tables[i])

    print(branches_tables_after_handle)



    # pop_list2 = []

    dependency_between_branches = []

    for i in range(len(branches_tables_after_handle)):
        for j in range(i+1,len(branches_tables_after_handle)):
            for item1 in branches_tables_after_handle[i]:
                for item2 in branches_tables_after_handle[j]:
                    if has_depedency(item1,item2,nodeID_of_edge):
                        if [i,j] not in dependency_between_branches:
                            dependency_between_branches.append([i,j])
                            break
    new_division = range(len(branches_tables_after_handle))
    new_division = list(new_division)

    for i in range(len(dependency_between_branches)-1,-1,-1):
        new_division[dependency_between_branches[i][1]] = new_division[dependency_between_branches[i][0]]
    # print('||||||||||||||||||||||||||')
    # print(new_division)
    unique_values = list(set(new_division))
    unique_values_index_list = []
    for i in unique_values:
        tmp_list = []
        for index,value in enumerate(new_division):
            if value == i:
                tmp_list.append(index)
        unique_values_index_list.append(tmp_list)

    print(unique_values_index_list)

    new_branches_tables_after_handle = []
    for item in unique_values_index_list:
        tmp_list = []
        for i in item:
            tmp_list.extend(branches_tables_after_handle[i])
        new_branches_tables_after_handle.append(tmp_list)

    print(new_branches_tables_after_handle)

    key_id = 0
    branchTables_dict = dict()
    for item in new_branches_tables_after_handle:
        branchTables_dict[key_id] = new_branches_tables_after_handle[key_id]
        key_id+=1
    print(branchTables_dict)

    branchFields_dict = dict()
    for key in branchTables_dict.keys():
        tmp_list = []
        for i in branchTables_dict[key]:
            value  = ellipse_nodeID_label_dict[i]
            field_list = controlBlock_table_filed_dict['ingress'][value]
            for j in field_list:
                if j not in tmp_list:
                    tmp_list.append(j)
        branchFields_dict[key] = tmp_list

    print(branchTables_dict)

    #---------------------------------时间互斥和逻辑互斥分析-------------------------------
    #---------------------------------table + fields--------------------------------
    print(nodeid_of_each_stage)
    print(tableID_label)
    print(branches_tables_after_handle)
    print(controlBlock_table_filed_dict)
    #----------------------求table中field的生命周期---------------------

    table_of_each_stage = []
    tables_list = []
    for item in nodeid_of_each_stage:
        table_list = []
        for i in item:
            if i in ellipse_nodeID:
                table_list.append(ellipse_nodeID_label_dict[i])
                tables_list.append(ellipse_nodeID_label_dict[i])
        table_of_each_stage.append(table_list)
    print(table_of_each_stage)

    tables_lifetime_dict = dict()
    fileds_lifetime_dict = dict()                     #初始化字典
    fields_list = []
    for i in tables_list:
        # i = 'ingress.'+ i
        tables_lifetime_dict[i] = []

        fields_list.extend(controlBlock_table_filed_dict['ingress'][i])

    # print(fields_list)
    for i in fields_list:
        fileds_lifetime_dict[i] = []
    print(nodeid_of_each_stage)
    print(ellipse_nodeID_label_dict)
    for index,i in enumerate(nodeid_of_each_stage):
        for item in i:
            # tables_lifetime_dict[].append(index)
            if item in ellipse_nodeID:
                node_name = ellipse_nodeID_label_dict[item]
                for field in controlBlock_table_filed_dict['ingress'][node_name]:
                    if index not in fileds_lifetime_dict[field]:
                        fileds_lifetime_dict[field].append(index)

    # print(new_branches_tables_after_handle)
    # print(branchTables_dict)
    # print(fileds_lifetime_dict)
    # print(branchFields_dict)
    return fileds_lifetime_dict,branchFields_dict




def run_parser(dot_file,json_file):
    dot_content,json_info = read_files(dot_file,json_file)
    nodeID_of_edge,edge_label,ellipse_nodeID,ellipse_nodeID_label_dict,ellipse_node_label,rectangle_nodeID,rectangle_node_label,rectangle_nodeID_label_dict = parse_dot_file(dot_content)
    controlBlock_table_filed_dict = parse_json(json_info)
    fileds_lifetime_dict,branchFields_dict = return_dict(nodeID_of_edge,edge_label,ellipse_nodeID,ellipse_node_label,rectangle_nodeID,rectangle_node_label,ellipse_nodeID_label_dict,rectangle_nodeID_label_dict,controlBlock_table_filed_dict)
    print(fileds_lifetime_dict)
    print(branchFields_dict)




run_parser('out/ingress.dot',"out/1.json")