import os

logs_path = 'logs/'
log_files = os.listdir(logs_path)

total_messages = {}         # Dictionary with total messages per iteration
received_messages = [{}, {}]      # List with the received messages per user

maxim = 0

total_sended = 0
total_received = 0
rec_0 = 0
rec_1 = 0

for log in log_files:
    # Get log id
    log_id = log.split(".")[0]

    # Read the log file and split the lines
    with open(logs_path + log) as f:
        lines = f.readlines()

    # Different action if it is one of the target files
    if log_id == "time":
        lines = [float(l.strip("\n")) for l in lines]
        # Find maximum time 
        for line in lines:
            if line > maxim:
                maxim = line
    elif log_id == "0" or log_id == "1":
        int_id = int(log_id)
        
        # Read the results file
        lines = [l.strip("\n").split(":")[-1] for l in lines]
        lines = [l.strip(" ").split("_") for l in lines]

        # Fill the lists
        for line in lines:
            sender = line[0]
            iteration = line[1]
            message = int(line[2])

            # Check if there is the id in the dictionary
            if not (sender in received_messages[int_id]):
                received_messages[int_id][sender] = {}

            # Check if there is the iteration 
            if iteration in received_messages[int_id][sender]:
                received_messages[int_id][sender][iteration].append(message)
                total_received += 1
            else:
                received_messages[int_id][sender][iteration] = [message]
                total_received += 1
            if int_id:
                rec_1 += 1
            else:
                rec_0 += 1
    else:
        ''' Make a new list which will have all the send messages per
            iteration in dictionaries with keys the iterations ids.
        ''' 
        total_messages[log_id] = [{}, {}]

        lines = [l.strip("\n").split("_") for l in lines]
        
        for line in lines:
            iteration = line[0]
            messages_0 = line[1]
            messages_1 = line[2]
            total_messages[log_id][0][iteration] = int(messages_0)
            total_messages[log_id][1][iteration] = int(messages_1)
            total_sended += int(messages_0) + int(messages_1)


identity = 0
total_lost = 0
print (rec_0, rec_1)
# Validate that got the right messages
for receiver in received_messages:
    for sender in receiver:
        for iteration in receiver[sender]:
            # Find the possible messages
            sended = total_messages[sender][identity][iteration]
            if not identity:
                must_be = range(2, 2*(sended + 1), 2)
            else:
                must_be = range(3, 2*(sended + 1), 2)
            
            # Count how many messages received
            for message in must_be:
                if not (message in receiver[sender][iteration]):
                    print ("Missing from %s from sender %s iteration %s mes %d"%(identity,
                                                                                sender,
                                                                                iteration,
                                                                                message))
                    total_lost += 1
            for message in receiver[sender][iteration]:
                if not (message in must_be):
                    print ("-Missing from %s from sender %s iteration %s mes %d"%(identity,
                                                                                sender,
                                                                                iteration,
                                                                                message))

    identity += 1

print ("Sended: %d\nReceived: %d\nLost: %d\nCheck: %d\n"%(total_sended,
                                                          total_received,
                                                          total_lost,
                                                          total_sended - total_received))
print ("Time: %f\n%f messages/second\n"%(maxim, total_received/maxim))

