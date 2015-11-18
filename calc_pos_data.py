import subprocess,pdb

ocv_small = 'output/ocv.data'
clsfr_small = 'output/10x32.data'
tp_count = 0
fp_count = 0
linecount = 0

matches = 0

true_pos = 'output/10x32_truepos.data'
false_pos = 'output/10x32_falsepos.data'

# for line in classifier
with open(clsfr_small) as f:

	fpfile = open(false_pos,'w')
	tpfile = open(true_pos,'w')

	no_lines = subprocess.check_output('sed -n \'$=\' '+clsfr_small, shell=True) 

	for line in f:
		linecount = linecount + 1
		if (len(line) < 5) or (linecount == 1):
		# if linecount < 326:
			continue

		# pdb.set_trace()
		# get classification
		sedcmd = 'sed \'' + str(linecount+1)+'q;d\' '+clsfr_small
		datalabel = subprocess.check_output(sedcmd, shell=True)

		if datalabel[0] == '1':
			print('searching for line '+str(linecount)+'/'+str(no_lines))
			# look in ocv
			commandline = ['grep', '-rZn', line.split('\n')[0]  ,ocv_small]

			try:
				result = subprocess.check_output(commandline)
				lineno = result.split(':')[1]
				ocv_sedcmd = 'sed \'' + str(int(lineno)+1)+'q;d\' '+ocv_small
				ocv_datalabel = subprocess.check_output(ocv_sedcmd, shell=True)
				if ocv_datalabel[0] == '1':
					tpfile.write(line)
					tpfile.write('1\n')
					tp_count = tp_count + 1
				else:
					fpfile.write(line)
					fpfile.write('0\n')
					fp_count = fp_count + 1

			except subprocess.CalledProcessError, e:
				continue

		if linecount%100 == 0:
			print('update: line'+str(linecount)+' of '+str(no_lines))
			print('true pos: '+str(tp_count))
			print('false pos: '+str(fp_count))

		# print('\t'+clsfr_small+':'+result.split(':')[1])
		# true_pos = true_pos + 1

	print("DONE!!!!!!!!!!!!")
	print('true pos: '+str(tp_count))
	print('false pos: '+str(fp_count))
	tpfile.close()
	fpfile.close()
