
# 數學運算重要的元件...
import numpy as np

'''
FUNCTION:
(A) 從2個數字/文字, 去計算 A,B數值
(B) 啟始作業: 取得所有可能解
(C) 計算各解的數量[0..5,0..5]
(D) 去除不符合解
(E) 在剩下可能解中, 取得一個任意解

MAIL:
1. 啟始作業(B), 取得可能解, 及工作陣例
2. LOOP
2.1 從剩下解中, 任取一個當做猜測(E)
2.2 輸入 A,B, 去除異常輸入
2.3 計算各種可能的數量(C)....可以發展出日後對猜的程式
2.4 陫除不符合 A,B的數(D), 保留可能解, (以供2.1取用)

B,C,D: 引用(A)
B:計算時 套入本身, 判斷B, B==0, 表示各位置都不同, B>0 表示必有一個位置數字重複
C,D: 計算AB
'''

# 透過 NUMPY 取得0...9999的陣例, NUMPY自動填入數值 0..9999
# 我全部的數字 0...9999
myNumber = np.arange(0, 10000, dtype=int)
# 刪去後剩下來的數字, remain(保有, 持有, 剩下)
remNumber = np.arange(0, 10000, dtype=int)


# PYTHON 特色, 可以回傳多個數值, 所以就回傳A,B 的數量
def countAB(guess, num):
    a = 0
    b = 0
    # print(type(guess))
    if type(guess) is str:
        g = list(guess)
    else:  # 非文字必需轉文字後再比對
        g = list("{:04d}".format(guess))

    if type(num) is str:
        n = list(num)
    else:  # 非文字必需轉文字後再比對
        n = list("{:04d}".format(num))
    for i in range(len(g)):
        for j in range(len(n)):
            if g[i] == n[j]:
                if i == j:
                    a = a+1  # 同位置, A+1
                if i != j:
                    b = b+1  # 不同位置, B+1
    return a, b


# 取得四個不相同的數字, 並填入 remNumber
def initAll():
    # GOLBAL: 參考全域變數...否則會當做區域(LOCAL)變數處理
    global myNumber, remNumber
    print("init all")

    for i in range(0, 10000):
        a, b = countAB(i, i)
        if b == 0:
            myNumber[i] = i  # 如果是可能解, 填入數值, 若為有重複不要的, 填入-1
        else:
            myNumber[i] = -1  # 如果是可能解, 填入數值, 若為有重複不要的, 填入-1

    # 因myNumber是numpy物件, 所以可以透過NUMPY中的過瀘功能取得在myNumber中非負的所有內容, 並産生成另一個陣例
    # remNumber(剩下來的可能解) = 從 myNumber 中找出 myNumber非負的 容, 産生新陣列
    remNumber = myNumber[myNumber >= 0]
    # 顯示結果, remNumber.shape: 外觀, 也就是陣列的結構
    print("length of remained", remNumber.shape[0])


# 計算可能解的數量....
# 排列組合可脦, A=0..4, b=0..4, 所以可能解為 [5*5]種情況 (為簡化處理, 筅不排除不合理的繻合, 線: A=2, B=3)
def countSolution(guess, numsArray):
    # print("length of nums:", len(numsArray))
    # 以NUMPY, 取得一個全 0 的陣例, 大小 25(=5*5*)
    solution = np.zeros(25, dtype=int)
    # 轉換 25 元素的陣例, 為 5*5 的陣例
    solution = np.reshape(solution, (5, 5))

    #錯誤檢查, 讓不合規定的內容不要處理
    if len(guess) != 4:
        return None
    a, b = countAB(guess, guess)
    if b > 0:
        return  # 當輸入在不同位置有相同字元時, 不再向下處理,,,不合理不處理

    # 計算傳入的陣列跟猜測內容的A,B總量
    # print("compare loop" ,x1,y1)
    for x in numsArray:
        if x == -1:
            continue  # 己經被排除的, 不是要計算的範圍
        a, b = countAB(x, guess)
        solution[a, b] = solution[a, b]+1
    return solution


# 刪除跟 A,B不同的解
def keepAB(guess, keepA, keepB):
    global myNumber, remNumber
    filltred = 0

    for i in remNumber:
        if i < 0:
            continue
        a, b = countAB(i, guess)

        if a != keepA or b != keepB:  # 刪除答案不對的
            myNumber[i] = -1
            filltred = filltred+1  # 刪除量加1
    # 過瀘保留的數值
    remNumber = myNumber[myNumber >= 0]
    if len(remNumber)>1:
        print(remNumber)
        print("Filter: deleted:", filltred, "keep:", remNumber.shape[0])

# 從剩下來的可能解, 回傳亂數下的某一個值....


def produceGuess():
    return np.random.choice(remNumber, 1)[0]

# 主程式


# 設定初始容,
# 取得一個0...9999的陣列, 不合的解以 -1 排除
# 取得可能解, 以做為日後猜答案的基礎
initAll()

# LOOP, 直到猜到 4A 為止
# 1. 從剩下來的數字中, 任選一個數
# 2. 計算各答案的可能性.....為了做統計用
# 3. 輸入猜的結果, 並做基本檢查, 直到輸入正確為止
# 3.1 檢查長度
# 3.2 檢查合理性
# 4. 排除不符合的解

# LOOP, 直到 4A為止, 即是剩下可能解的個數==1, 或完全沒有..
loopCount=0
while remNumber.shape[0] > 1:
    loopCount=loopCount+1
    # 1. 先從可能解中, 任取一個數字來猜
    num = produceGuess()
    cmputerGuess = "{:04d}".format(num)
    print("random guess=", cmputerGuess)

    # 2.計算各種A,B可能解數量01
    s = countSolution(cmputerGuess, myNumber)
    # m=s.argmax(axis=0)

    # 取得可能解數量中, 最大的那一個....WHY? 因為要保留最多的數量下來,好讓USER要猜更多次
    # 當要互猜時, 這個會用到...
    i, j = np.unravel_index(s.argmax(), s.shape)
    # print(m,i,j)
    print(s)

    # 3. 輸入猜的結果
    #
    abCount = ""
    aCount = -1
    bCount = -1
    while True:
        abCount = input("{}: my guess is *{}*, what is the result(AB))  ".format(loopCount,cmputerGuess))
        if len(abCount) != 2:
            continue
        try:
            abCount = list(abCount)
            aCount = int(abCount[0])  # 轉換錯誤時, 進入except(例外)處理
            bCount = int(abCount[1])  # 轉換錯誤時, 進入except(例外)處理
            if(aCount+bCount > 4):
                continue  # 總數不該>4
        except:  # 如果處理有錯時, 會執行例外處置
            # 可能輸入為文字, 可能輸入負號, ....轉換數值時出錯...會進入這個地方
            print("Input wrong, try again")
            continue
            pass
        print("you response=", aCount, bCount)
        break

    # 4. 排除錯誤的可能答案
    keepAB(cmputerGuess, aCount, bCount)
    # break
print("total guess times:",loopCount)
pass

