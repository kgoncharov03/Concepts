import re


def select(seq, func):
    def new_seq():
        for elem in seq:
            yield func(elem)
    return new_seq()


def flatten(seq):
    def new_seq():
        for elem_seq in seq:
            for elem in elem_seq:
                yield elem
    return new_seq()


def where(seq, func):
    def new_seq():
        for elem in seq:
            if func(elem):
                yield elem
    return new_seq()


def take(seq, k):
    def new_seq():
        counter = 0
        for elem in seq:
            yield elem
            counter += 1
            if counter == k:
                break
    return new_seq()


def group_by(seq, func):
    def new_seq():
        seq_dict = {}
        for elem in seq:
            tmp_key = func(elem)
            if tmp_key in seq_dict:
                seq_dict[tmp_key].append(elem)
            else:
                seq_dict[tmp_key] = [elem]
        for key in seq_dict:
            yield [key, seq_dict[key]]
    return new_seq()


def order_by(seq, func=None):
    def new_seq():
        seq_list = list(seq)
        seq_list.sort(key=func)
        for elem in seq_list:
            yield elem
    return new_seq()


class Sequence:
    def __init__(self, seq=()):
        self.seq = seq

    def ToList(self):
        return list(self.seq)

    def Select(self, func):
        return Sequence(select(self.seq, func))

    def Flatten(self):
        return Sequence(flatten(self.seq))

    def Where(self, func):
        return Sequence(where(self.seq, func))

    def Take(self, k):
        return Sequence(take(self.seq, k))

    def GroupBy(self, func):
        return Sequence(group_by(self.seq, func))

    def OrderBy(self, func=None):
        return Sequence(order_by(self.seq, func))


def fib():
    x = 1
    y = 1
    while 1:
        x, y = y, y + x
        print(x)
        yield x


def words():
    txt = open('shakespeare.txt', 'r')
    for line in txt.readlines():
        for word in filter(None, line.strip().split(' ')):
            yield re.sub(r'\W', '', word.lower())


print(Sequence(fib()).Where(lambda x: x % 3 == 0).Select(lambda x: x**2 if x % 2 == 0 else x).Take(5).ToList())
print(Sequence(words()).GroupBy(lambda x: x).Select(lambda x: [x[0], len(x[1])]).OrderBy(lambda x: 1/x[1]).ToList())





