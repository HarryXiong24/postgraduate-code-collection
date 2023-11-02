import numpy as np


def write_distance_matrix(n, mean, sigma):
    distance_matrix = np.zeros((n, n))
    random_distance = []
    num_distance = int(n * (n-1) / 2)
    for _ in range(num_distance):
        distance = 0
        while distance <= 0:
            distance = np.random.normal(mean, sigma)

        random_distance.append(distance)
    
    iu = np.triu_indices(n, 1)
    distance_matrix[iu] = random_distance
    distance_matrix += distance_matrix.T

    np.savetxt(
        f"{n}_{mean}_{sigma}.out",
        distance_matrix,
        delimiter=" ",
        fmt="%1.4f",
        header=str(n),
        comments="",
    )


if __name__ == "__main__":
    n = int(input("Enter the number of locations: "))
    mean = float(input("Enter the mean: "))
    sigma = float(input("Enter the standard deviation: "))

    write_distance_matrix(n, mean, sigma)
