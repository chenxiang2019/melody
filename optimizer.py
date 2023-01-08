from gurobipy import * 
import time

def melody(field_cnt, field_width, word_cnt, word_width, L, E):
    m = Model()
    x = m.addVars(field_cnt, word_cnt, vtype=GRB.BINARY)
    y = m.addVars(1, word_cnt, vtype=GRB.BINARY)
    z = m.addVars(field_cnt, word_cnt, vtype=GRB.INTEGER)
    max_field_width = m.addVars(1, word_cnt, vtype=GRB.INTEGER)
    obj = m.addVar(vtype=GRB.INTEGER)
    m.setParam("SolutionLimit", 100)
    m.setParam('OutputFlag', 0)
    # for i in range(field_cnt):
    #     m.addConstr(  quicksum( [x[i, j] for j in range(word_cnt) ] ) == 1 )

    # for j in range(word_cnt): 
    #     m.addConstr(  quicksum( [x[i, j] for i in range(field_cnt) ] ) == 1 )

    for i in range(field_cnt):
        for j in range(word_cnt):
            m.addConstr(  (  x[i, j] == 1 ) >> (z[i, j] >= 1 )  )
            m.addConstr(  (  x[i, j] == 0 ) >> (z[i, j] == 0 )  )

    for i in range(field_cnt):
        m.addConstr(  quicksum(   [  z[i, j] for j in range(word_cnt)  ]    )  == field_width[i]  )

    for i in range(field_cnt):
        for j in range(field_cnt):
            if L[i][j] == 0 and E[i][j] == 0 and i < j:
                for k in range(word_cnt):
                    m.addConstr(  (x[i, k] == 1) >> ( x[j, k]  == 0 )  )
                    m.addConstr(  (x[j, k] == 1) >> ( x[i, k]  == 0 )  )


    for j in range(word_cnt):
        m.addGenConstrMax(max_field_width[0, j], [ z[i, j] for i in range(field_cnt)])

    for j in range(word_cnt):
        m.addConstr(  max_field_width[0, j]   <= word_width[j]  )

    for j in range(word_cnt):
        m.addGenConstrOr( y[0, j] , [  x[i, j] for i in range(field_cnt) ] )
    
    m.addConstr( obj == quicksum(  [  y[0, j] * word_width[j] for j in range(word_cnt) ] ) )

    m.setObjective(obj, GRB.MINIMIZE)

    strat = time.time()
    m.optimize()
    end = time.time()

    return obj.X, (end - strat) * 1000

    