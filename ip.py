import cv2
import numpy as np
import pytesseract

# Function to extract and recognize digits
def extract_digits(image):
    cells = []
    for i in range(9):
        row = []
        for j in range(9):
            x, y = j * 50, i * 50
            cell = image[y:y+50, x:x+50]
            cell = cv2.resize(cell, (28, 28))
            cell = cell.astype('uint8')  # Convert to uint8
            row.append(cell)
        cells.append(row)
    return cells

# Function to check if a number can be placed in the Sudoku grid
def is_valid(board, row, col, num):
    for i in range(9):
        if board[row][i] == num or board[i][col] == num:
            return False
    start_row, start_col = 3 * (row // 3), 3 * (col // 3)
    for i in range(start_row, start_row + 3):
        for j in range(start_col, start_col + 3):
            if board[i][j] == num:
                return False
    return True

# Function to solve the Sudoku puzzle using backtracking
def solve_sudoku(board):
    for row in range(9):
        for col in range(9):
            if board[row][col] == 0:
                for num in range(1, 10):
                    if is_valid(board, row, col, num):
                        board[row][col] = num
                        if solve_sudoku(board):
                            return True
                        board[row][col] = 0
                return False
    return True

# Function to write the solved Sudoku solution back to the image
def write_solution(image, solved_grid):
    for i in range(9):
        for j in range(9):
            if sudoku_grid[i][j] == 0:  # Only write the solved digits
                text = str(solved_grid[i][j])
                x, y = j * 50 + 15, i * 50 + 35
                cv2.putText(image, text, (x, y), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2, cv2.LINE_AA)

# Read the image
image = cv2.imread('sudoku.jpg', cv2.IMREAD_GRAYSCALE)

# Preprocess the image
blurred = cv2.GaussianBlur(image, (5, 5), 0)
thresh = cv2.adaptiveThreshold(blurred, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, cv2.THRESH_BINARY_INV, 11, 2)

# Find contours
contours, _ = cv2.findContours(thresh, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

# Find the largest contour
largest_contour = max(contours, key=cv2.contourArea)

# Approximate the contour to a polygon
epsilon = 0.02 * cv2.arcLength(largest_contour, True)
approx = cv2.approxPolyDP(largest_contour, epsilon, True)

# Ensure the contour is a rectangle
if len(approx) == 4:
    # Draw the contour
    cv2.drawContours(image, [approx], 0, (0, 255, 0), 3)
    pts = np.float32([approx[0], approx[1], approx[2], approx[3]])
    pts2 = np.float32([[0, 0], [450, 0], [450, 450], [0, 450]])
    matrix = cv2.getPerspectiveTransform(pts, pts2)
    result = cv2.warpPerspective(image, matrix, (450, 450))
else:
    print("Sudoku grid not found.")
    result = image

# Extract digits
digits = extract_digits(result)

# Recognize digits using Tesseract OCR
sudoku_grid = []
for row in digits:
    row_digits = []
    for cell in row:
        cell = cv2.cvtColor(cell, cv2.COLOR_GRAY2BGR)
        digit = pytesseract.image_to_string(cell, config='--psm 10 digits')
        row_digits.append(int(digit) if digit.isdigit() else 0)
    sudoku_grid.append(row_digits)

# Print the recognized Sudoku grid
print("Recognized Sudoku Grid:")
for row in sudoku_grid:
    print(row)

# Solve the Sudoku puzzle
solve_sudoku(sudoku_grid)

# Print the solved Sudoku grid
print("Solved Sudoku Grid:")
for row in sudoku_grid:
    print(row)

# Write the solution
write_solution(result, sudoku_grid)

# Show the solved Sudoku
cv2.imshow('Solved Sudoku', result)
cv2.waitKey(0)
cv2.destroyAllWindows()
