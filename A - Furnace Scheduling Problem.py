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

# 時刻ごとの不稼働マシンリスト作成
invalid_machine_lst = [[] for i in range(8640000)]
for idx, m in enumerate(machine_info):
    for day in m[3:]:
        start = day * 86400
        end = start + 86400
        for i in range(start, end):
            invalid_machine_lst[i].append(idx)

t_duration = dt.datetime.now() - t_begin
print(t_duration)

# github test
print(t_duration)
a = 0