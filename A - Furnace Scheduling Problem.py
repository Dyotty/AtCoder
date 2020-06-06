def get_input_int():
    row = map(str, input().split())
    return [int(val) for val in list(row)[1:]]


import datetime as dt


n_machine, n_bread, num_order, BOM_row_num, COMBI_row_num = get_input_int()
t_begin = dt.datetime.now()
machine_info = [get_input_int()[1:] for i in range(n_machine)]
BOM_info = [get_input_int() for i in range(BOM_row_num)]
COMBI_info = [get_input_int() for i in range(COMBI_row_num)]
order_info = [get_input_int()[1:] for i in range(num_order)]

"""
・まずは制約条件を満たせること
・最適化はどうやって行うか？
    方式1）局所最適を繰り返す
        優位点：計算時間が少ない
        問題：正しく最適化されない可能性が高い
        方式１−１）全体最適は行わない
        方式１ー2）１の後に全体最適化処理を加える
                問題：できるかわからない
    方式２）初めから全体最適
        優位点：正しく最適化される可能性が高い
        問題：計算に時間がかかる
        
・判断要素
    ・各オーダーは最早開始時刻と納期が決まっており、各パラメータの取りうる範囲には限りがある
    ・オーダーの分割が可能なので、単位時間焼き数は詰めやすい
    ・しかし、分割が可能な分、パターンが膨大になる
        →　基本的に分割は全て分割して考え、同じバッチに複数ある場合は出力時に結合する
        
・方針
    １．とりあえず制約を満たせるように作る
    ２．その後、局所最適化を行う
    
    利益の最大化は以下の考え方で行う
        １．各オーダーの完了ができるだけ早くなるように
        　（納期に遅れなければペナルティはないので、出来るだけ遅れさせる方が良い？）
        ２．バッチごとのパンの量を多くする
        ３．段取りを少なくする
"""

def sec_to_day(sec):
    return float(sec) / 86400.0


def day_to_sec(day):
    return day * 86400


# 日ごとの使えないマシンリスト
invalid_machine_lst = [[] for day in range(1000)]
for m_idx, m_info in enumerate(machine_info):
    for rest_day in m_info[2:]:
        invalid_machine_lst[rest_day].append(m_idx)

# BOMをパンの種類ごとに整理
pan_info_lst = [{} for i in range(n_bread)]
for bom in BOM_info:
    pan_info_lst[bom[0]][bom[1]] = bom[2:]

# 計画表ベースを作成
schedule = {}
for i in range(n_machine):
    schedule[i] = []
for m_idx, m_info in enumerate(machine_info):
    for rest_day in m_info[2:]:
        start = day_to_sec(rest_day)
        end = start + 86399
        schedule[m_idx].append([start, end, 2])


# オーダーを最小単位に分割
def divide_order(order):
    pan_No = order[0]
    max_early_time = order[1]
    deadline = order[2]
    production_volume = order[3]
    min_volume_size = order[4]
    delay_time = order[6]

    # オーダーを極力分割
    mini_order = order
    mini_order[3] = min_volume_size
    div_order_lst = [mini_order for i in range(production_volume // min_volume_size)]
    div_order_lst[-1][3] += production_volume % min_volume_size
    return div_order_lst


# 計画表を参照しオーダーを割り当て
def assign_order(order):
    pan_No = order[0]
    max_early_time = order[1]
    deadline = order[2]
    production_volume = order[3]
    delay_time = order[6]

    for m in range(n_machine):
        production_time = pan_info_lst[pan_No][m][0]
        end_earliest_time = max_early_time + production_time
        production_day_lst = [sec_to_day(max_early_time), sec_to_day(end_earliest_time)]




# オーダーを読み出し、使えるマシンに割当て
for o in order_info:
    div_orders = divide_order(o)

    # 割り当てられるマシンを探索
    # get batch_time for machine


a = 0