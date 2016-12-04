function sleep(secs)
    print("i should sleep", secs, "seconds... nah")
    return secs
end

function fgets(buff, count, stream)
    print(string.format("someone called fgets. buff= %s count= %d", buff, count))
    return buff
end

function malloc(size)
    ret = dermis_malloc(size)
    if size == 10 then
        print(string.format("malloc was called for size: %d", size))
        print(string.format("called real malloc, returned value: %s", tostring(ret)))
    end
    return ret
end

dermis_register_sleep(sleep)
dermis_register_fgets(fgets)
dermis_register_malloc(malloc)
