import gdb

class ListSlabs(gdb.Command):
    
    def __init__(self):
        super(ListSlabs, self).__init__("slabs", gdb.COMMAND_USER)
    def invoke(self, arg, from_tty):
        try:
            main_cache = gdb.parse_and_eval("main_cache")
            
            bucket_sizes = [8, 16, 32, 64, 128, 256, 512, 1024, 2048]
            
            print("=" * 80)
            print("SLAB ALLOCATOR STATE")
            print("=" * 80)
            
            total_slabs = 0
            
            for i in range(len(bucket_sizes)): 
                slab_ptr = main_cache['buckets'][i]
                
                if int(slab_ptr) == 0:
                    continue
                
                print(f"\n[Bucket {i}] Size: {bucket_sizes[i]} bytes")
                print("-" * 80)
                
                slab_count = 0
                
                while int(slab_ptr) != 0:
                    slab = slab_ptr.dereference()
                    
                    slab_id = int(slab['id'])
                    size = int(slab['size'])
                    free_count = int(slab['free_count'])
                    max_count = int(slab['max'])
                    next_ptr = slab['next']
                    freelist = slab['freelist_head']
                    
                    used = max_count - free_count
                    usage_pct = (used / max_count * 100) if max_count > 0 else 0
                    
                    print(f"  Slab #{slab_count} @ {slab_ptr}")
                    print(f"    ID: {slab_id}")
                    print(f"    Size: {size} bytes")
                    print(f"    Usage: {used}/{max_count} ({usage_pct:.1f}%)")
                    print(f"    Free: {free_count}")
                    print(f"    Freelist: {freelist}")
                    print()
                    
                    slab_count += 1
                    total_slabs += 1
                    slab_ptr = next_ptr
                
                print(f"  Total slabs in bucket: {slab_count}")
            
            print("=" * 80)
            print(f"Total slabs: {total_slabs}")
            print("=" * 80)
            
        except gdb.error as e:
            print(f"err: {e}")

class InspectSlab(gdb.Command):
    
    def __init__(self):
        super(InspectSlab, self).__init__("vslab", gdb.COMMAND_USER)
    
    def invoke(self, arg, from_tty):
        if not arg:
            print("usage: vslab <address>")
            return
        
        try:
            slab_addr = gdb.parse_and_eval(arg)
            slab_ptr = gdb.Value(slab_addr).cast(gdb.lookup_type("struct mk_slab_t").pointer())
            slab = slab_ptr.dereference()
            
            slab_id = int(slab['id'])
            size = int(slab['size'])
            free_count = int(slab['free_count'])
            max_count = int(slab['max'])
            next_ptr = slab['next']
            freelist = slab['freelist_head']
            
            used = max_count - free_count
            usage_pct = (used / max_count * 100) if max_count > 0 else 0
            
            print("\n" + "=" * 80)
            print(f"SLAB INSPECTION @ {slab_ptr}")
            print("=" * 80)
            print(f"ID:           {slab_id}")
            print(f"Object Size:  {size} bytes")
            print(f"Max Objects:  {max_count}")
            print(f"Used:         {used} ({usage_pct:.1f}%)")
            print(f"Free:         {free_count}")
            print(f"Next Slab:    0x{int(next_ptr):016x}")
            print(f"Freelist:     0x{int(freelist):016x}")
            
            free_objects = []
            current_free = freelist
            while int(current_free) != 0:
                free_objects.append(int(current_free))
                try:
                    next_free_ptr = gdb.parse_and_eval(f"*(uint64_t*)0x{int(current_free):x}")
                    current_free = next_free_ptr
                except:
                    break
            
            slab_struct_size = 32  
            data_start = int(slab_ptr) + slab_struct_size
            
            page_size = 4096
            page_start = int(slab_ptr) & ~(page_size - 1)
            
            print(f"\n" + "=" * 80)
            print(f"MEMORY DUMP (Page @ 0x{page_start:016x})")
            print("=" * 80)
            
            inferior = gdb.selected_inferior()
            
            COLOR_RESET = "\033[0m"
            COLOR_GREEN = "\033[92m"      
            COLOR_RED = "\033[91m"      
            COLOR_BLUE = "\033[94m"      
            COLOR_YELLOW = "\033[93m"
            COLOR_CYAN = "\033[96m"
            COLOR_MAGENTA = "\033[95m"
            
            OBJECT_COLORS = [COLOR_RED, COLOR_GREEN, COLOR_CYAN, COLOR_MAGENTA, "\033[91m", "\033[92m"]

            bytes_per_line = 16  
            
            for offset in range(0, page_size, bytes_per_line):
                addr = page_start + offset
                
                try:
                    memory = inferior.read_memory(addr, bytes_per_line)
                    byte_array = [ int.from_bytes(b, "little") for b in memory ]

                except gdb.MemoryError:
                    print(f"{COLOR_YELLOW}0x{addr:016x}{COLOR_RESET}  ???????????????? ????????????????  ????????????????")
                    continue

                except Exception as e:
                    print(f"{COLOR_YELLOW}0x{addr:016x}{COLOR_RESET}  [err: {e}]")
                    continue
                
                is_metadata = addr < data_start
                
                print(f"{COLOR_YELLOW}0x{addr:016x}{COLOR_RESET}  ", end="")
                
                for word_idx in range(2):
                    word_offset = word_idx * 8
                    word_addr = addr + word_offset
                    
                    if is_metadata:
                        color = COLOR_BLUE
                    else:
                        obj_idx = (word_addr - data_start) // size
                        if obj_idx >= 0 and obj_idx < max_count:
                            obj_addr = data_start + (obj_idx * size)
                            color = OBJECT_COLORS[obj_idx % len(OBJECT_COLORS)]
                        else:
                            color = COLOR_RESET
                    
                    if word_offset + 8 <= len(byte_array):
                        word_bytes = byte_array[word_offset:word_offset+8]
                        print(f"{color}", end="")
                        for i in range(7, -1, -1):  # little endian 
                            print(f"{word_bytes[i]:02x}", end="")
                        print(f"{COLOR_RESET} ", end="")
                    else:
                        print("???????????????? ", end="")
                
                print(" ", end="")
                
                for i in range(bytes_per_line):
                    if i < len(byte_array):
                        byte = byte_array[i]
                        byte_addr = addr + i
                        
                        if is_metadata:
                            color = COLOR_BLUE
                        else:
                            obj_idx = (byte_addr - data_start) // size
                            if obj_idx >= 0 and obj_idx < max_count:
                                color = OBJECT_COLORS[obj_idx % len(OBJECT_COLORS)]
                            else:
                                color = COLOR_RESET
                        
                        if 32 <= byte <= 126:
                            print(f"{color}{chr(byte)}{COLOR_RESET}", end="")
                        else:
                            print(f"{color}.{COLOR_RESET}", end="")
                    else:
                        print(" ", end="")
                
                if addr in free_objects:
                    print("  <=== isfree", end="")
                
                print()
            
            print("\n" + "=" * 80)
            
        except gdb.error as e:
            print(f"Error: {e}")

ListSlabs()
InspectSlab()

