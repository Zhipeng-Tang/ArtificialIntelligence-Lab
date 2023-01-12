import os

input = ['input00.txt', 'input01.txt', 'input02.txt', 'input03.txt', 'input04.txt', 'input05.txt', 'input06.txt', 'input07.txt', 'input08.txt', 'input09.txt', 'input10.txt', 'input11.txt']
target = ['target00.txt', 'target01.txt', 'target02.txt', 'target03.txt', 'target04.txt', 'target05.txt', 'target06.txt', 'target07.txt', 'target08.txt', 'target09.txt', 'target10.txt', 'target11.txt']
file = ['output_A_h1.txt', 'output_A_h2.txt', 'output_IDA_h1.txt', 'output_IDA_h2.txt']
function = ['A_h1', 'A_h2', 'IDA_h1', 'IDA_h2']

for f in file:
    os.system('del {}'.format(f))

for func in function:
    for i in range(0, len(input)):
        cmd = '.\\a.exe {} {} {}'.format(func, input[i], target[i])
        os.system(cmd)