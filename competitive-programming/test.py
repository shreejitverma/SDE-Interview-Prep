# from sympy import symbols, divisors
# from sympy.solvers import solve

# # Define the variable and equation
# x = symbols("x")
# equation = x**20 - k * x - 22 * k

# # Initialize a counter for positive integer solutions
# count = 0

# # Iterate over possible values of k
# for k in range(1, 1000):  # Adjust the range as needed
#     # Find roots of the equation for the current value of k
#     roots = solve(equation.subs(k=k), x)
#     # Check if roots are positive integers
#     for root in roots:
#         if root.is_integer and root > 0:
#             count += 1

# print("Number of positive integers x:", count)


from scipy.stats import chi2
from scipy.optimize import fsolve
import numpy as np


def equation_to_solve(x):
    return x * np.log(1 - chi2.cdf(0.5, 2)) - 1


# Initial guess for x
initial_guess = 1.0

# Solving the equation numerically
solution = fsolve(equation_to_solve, initial_guess)

print("The solution for x is:", solution[0])
