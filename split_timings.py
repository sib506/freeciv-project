
import collections

# start = {'selection': [1,2,3], ...}
start = collections.defaultdict(list)
end = collections.defaultdict(list)

with open("time_log_1_iter.txt") as f:
	for line in f:
		subject, time = line.strip().split(":")
		prefix, _, name = subject.partition(' ')
		if prefix == 'Start':
			start[name].append(int(time))
		else:
			end[name].append(int(time))
	
# start {'selection': [3, 4]}, end {'selection': [5, 6]}
# create: {'selection': sum(b - a for (a, b) in [(5,3), (6,4)])}
deltas = {k: [b - a for (b, a) in zip(end[k], start[k])] for k in start}

#times = {k: sum(deltas[k]) for k in deltas}

times = {}
for k in deltas:
	total = 0
	for v in deltas[k]:
		if v != 0:
			total += v
		else:
			total += 0.5	
	times[k] = total

import pprint

pprint.pprint([(k, v) for (k, v) in sorted(times.items(), key=lambda v: v[1])])
