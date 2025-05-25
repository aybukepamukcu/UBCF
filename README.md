# 🎬 User-Based Collaborative Filtering (UBCF) in C++

This project implements a **User-Based Collaborative Filtering** algorithm using **cosine similarity** to predict movie ratings based on user similarities.

It reads a training dataset of user–movie–rating triples and a test dataset of user–movie pairs, then outputs predicted ratings for the test cases.

---

## 📂 Input Format

The program reads from `stdin` and expects the following structure:

```
train dataset  
userID movieID rating  
userID movieID rating  
...  
test dataset  
userID movieID  
userID movieID  
...
```

Example:
```
train dataset  
1 10 4.5  
1 12 3.0  
2 10 5.0  
test dataset  
1 11  
2 12
```

---

## ⚙️ Features

- Calculates user average ratings  
- Uses **cosine similarity** for user-to-user similarity  
- Ignores users with low overlap or low similarity (threshold > 0.05)  
- Considers top-`k` similar users (`k=30` by default)  
- Falls back to global average when no prediction is possible  

---

## 🧠 Algorithms

- **Cosine Similarity**  
  Used to compare the normalized rating vectors of users (centered by user average).

- **Prediction Formula**  
  Predicted rating is calculated as:

```
predicted_rating = target_user_avg + Σ(similarity * rating_diff) / Σ(abs(similarity))
```

---

## 🧪 Example Output

Given a test pair `1 11`, if enough similar users rated movie 11, it might output:

```
3.8
```

---

## 🛠 Compile and Run

```bash
g++ -std=c++11 -O2 ubcf.cpp -o ubcf
./ubcf < input.txt > output.txt
```

---
