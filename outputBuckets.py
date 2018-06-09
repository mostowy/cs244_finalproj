
total_size = 1610612736

# Intro 
print "192 MB is " + str(total_size) + " in bits."
print"-------------------------------------------"

# Quotient Filter
r_val = 9
size_per_bucket = r_val + 3;
buckets = int(total_size / size_per_bucket)
print "For Quotient Filter with r size " + str(r_val) + " use "  + str(buckets) + " buckets."

# Cuckoo Filter
f_val = 12
b = 4
size_per_bucket = f_val * b
buckets = int(total_size / size_per_bucket)
print "For Cuckoo Filter with f size " + str(f_val) + " use "  + str(buckets) + " buckets."


# Cuckoo Filter
f_val = 12
b = 4
size_per_bucket = ((f_val - 4) * b) + 12
buckets = int(total_size / size_per_bucket)
print "For Cuckoo Filter with f size " + str(f_val) + " use "  + str(buckets) + " buckets."
