def match(pattern: str, s: str) -> bool:
    container = s.split()
    mapper = dict({})

    if len(pattern) != len(container):
        return False

    for i in range(0, len(pattern)):
        if pattern[i] not in mapper and container[i] not in mapper.values():
            mapper[pattern[i]] = container[i]
        elif pattern[i] not in mapper and container[i] in mapper.values():
            return False
        else:
            if container[i] != mapper[pattern[i]]:
                return False

    return True


# test
res = match("abba", "dog cat cat dog")
print(res)
