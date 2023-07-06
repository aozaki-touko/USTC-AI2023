#!/usr/bin/env python3
import torch
import torch.nn as nn
import torch.nn.functional as F
from typing import List
import matplotlib.pyplot as plt

class char_tokenizer:
    """
    a very simple char-based tokenizer. the tokenizer turns a string into a list of integers.
    """

    def __init__(self, corpus: List[str]):
        self.corpus = corpus
        # TODO: calculate the vocab size and create a dictionary that maps each character to a unique integer
        self.char2idx = {char: idx for idx, char in enumerate(corpus)}
        self.idx2char = {idx: char for idx, char in enumerate(corpus)}
        self.n_vocab = len(self.corpus)
        # End of your code

    def encode(self, string: str):
        # TODO: convert a string into a list of integers and return, using the dictionary you created above
        return [self.char2idx[char] for char in string]
        # End of your code

    def decode(self, codes: List[int]):
        # TODO: convert a list of integers into a string and return, using the dictionary you created above
        return ''.join([self.idx2char[code] for code in codes])
        # End of your code


class Head(nn.Module):
    """single head of self-attention"""

    def __init__(self, head_size):
        super().__init__()
        # TODO: create three linear layers, Key, Query, and Value, each of which maps from n_embd to head_size
        #       and assign them to self.Key, self.Query, and self.Value, respectively
        self.Key = nn.Linear(n_embd, head_size)
        self.Query = nn.Linear(n_embd, head_size)
        self.Value = nn.Linear(n_embd, head_size)
        # End of your code
        self.register_buffer("tril", torch.tril(torch.ones(block_size, block_size)))
        self.dropOut = nn.Dropout(dropOut)
    def forward(self, inputs):
        # TODO: implement the forward function of the head
        #       the input is a tensor of shape (batch, time, n_embd)
        #       the output should be a tensor of shape (batch, time, head_size)
        #       you may use the tril buffer defined above to mask out the upper triangular part of the affinity matrix
        K = self.Key(inputs)
        Q = self.Query(inputs)
        V = self.Query(inputs)
        _, T, _ =inputs.shape
        attention = torch.matmul (Q,K.transpose(-2, -1)) / K.shape[-1]**0.5
        w = attention.masked_fill(self.tril[:T, :T] == 0, float("-inf"))
        w = F.softmax(w, dim=-1)
        w = self.dropOut(w)
        out = torch.matmul(w,V)
        # End of your code
        return out


class MultiHeadAttention(nn.Module):
    def __init__(self, n_heads, head_size):
        super().__init__()
        # TODO: implement heads and projection
        self.heads = nn.ModuleList([Head(head_size) for _ in range(n_heads)])
        self.projection = nn.Linear(n_heads * head_size, n_embd)
        self.dropOut = nn.Dropout(dropOut)
        # End of your code

    def forward(self, inputs):
        # TODO: implement the forward function of the multi-head attention
        out = torch.cat([head(inputs) for head in self.heads], dim=-1)
        out = self.projection(out)
        out = self.dropOut(out)
        return out


class FeedForward(nn.Module):
    def __init__(self, n_embd):
        super().__init__()
        # TODO: implement the feed-forward network

        self.net = nn.Sequential(
            nn.Linear(n_embd, 4 * n_embd),
            nn.ReLU(),
            nn.Linear(4 * n_embd, n_embd),
        )

        # End of your code

    def forward(self, inputs):
        return self.net(inputs)


class Block(nn.Module):
    def __init__(self, n_embd, n_heads):
        super().__init__()
        # TODO: implement the block of transformer using the MultiHeadAttention and
        # FeedForward modules, along with the layer normalization layers
        self.layerNorm1 = nn.LayerNorm(n_embd)
        self.layerNorm2 = nn.LayerNorm(n_embd)
        self.attention = MultiHeadAttention(n_heads, n_embd)
        self.feedForward = FeedForward(n_embd)
        # End of your code

    def forward(self, inputs):
        # TODO: implement the forward function of the block, you may refer to the docs of this experiment
        inputs = inputs + self.attention(self.layerNorm1(inputs))
        inputs = inputs + self.feedForward(self.layerNorm2(inputs))
        return inputs
        # End of your code


class Transformer(nn.Module):
    def __init__(self):
        super().__init__()
        # TODO: create the embedding table, the stack of blocks, the layer normalization layer,
        # and the linear layers.
        self.embedding = nn.Embedding(n_vocab, n_embd)
        self.position_embedding = nn.Embedding(block_size, n_embd)
        self.blocks = nn.Sequential(*[Block(n_embd, n_heads) for _ in range(n_layers)])
        self.norm = nn.LayerNorm(n_embd)
        self.linear = nn.Linear(n_embd, n_vocab)

    def forward(self, inputs, labels=None):
        # TODO: implement the forward function of the transformer
        _, time = inputs.shape
        tokenEmbeddings = self.embedding(inputs)
        posEmbeddings = self.position_embedding(torch.arange(time, device=device))
        out = tokenEmbeddings + posEmbeddings
        out = self.blocks(out)
        out = self.norm(out)
        logits = self.linear(out)
        if labels is None:
            loss = None
        else:
            batch, time, channel = logits.shape
            logits = logits.view(batch * time, channel)
            labels = labels.view(batch * time)
            loss = F.cross_entropy(logits, labels)
        return logits, loss

    def generate(self, inputs, max_new_tokens):
        # TODO: generate new tokens from the transformer, using the inputs as the context,
        #  and return the generated tokens with length of max_new_tokens
        for _ in range(max_new_tokens):
            inputs_crop = inputs[:, -block_size:]
            logits, loss = self(inputs_crop)
            # logits是一个(batch, time, n_vocab)的tensor，表示每个位置每个词的概率
            logits = logits[:, -1, :]
            probs = F.softmax(logits, dim=-1)
            inputs_next = torch.multinomial(probs, num_samples=1)
            inputs = torch.cat([inputs, inputs_next], dim=1)
        return inputs

def get_batch(split):
    data = train_data if split == "train" else val_data
    ix = torch.randint(len(data) - block_size, (batch_size,))
    x = torch.stack([data[i: i + block_size] for i in ix])
    y = torch.stack([data[i + 1: i + block_size + 1] for i in ix])
    x, y = x.to(device), y.to(device)
    return x, y


@torch.no_grad()
def estimate_loss(model):
    out = {}
    model.eval()
    for split in ["train", "val"]:
        losses = torch.zeros(eval_iters)
        for k in range(eval_iters):
            x, y = get_batch(split)
            logits, loss = model(x, y)
            losses[k] = loss.item()
        out[split] = losses.mean()
    return out


def generate(model):
    text = """Dear Terry,
\nHow are you doing? Learning that you are about to pay a visit to a Chinese friend
and confused about the Chinese customs, I am writing to put forward some advice.
"""
    context = torch.tensor([encode(text)], device=device, dtype=torch.long)
    print(decode(model.generate(context, max_new_tokens=500)[0].tolist()))


def train(model):
    optimizer = torch.optim.AdamW(model.parameters(), lr=learning_rate)
    train_losses = []
    val_losses = []
    for iter in range(max_iters):

        if iter % eval_interval == 0:
            losses = estimate_loss(model)
            train_loss = losses['train']
            val_loss = losses['val']
            train_losses.append(train_loss)
            val_losses.append(val_loss)
            print(
                f"step {iter}: train loss {losses['train']:.4f}, val loss {losses['val']:.4f}"
            )

        inputs, labels = get_batch("train")

        logits, loss = model(inputs, labels)
        optimizer.zero_grad(set_to_none=True)
        loss.backward()
        optimizer.step()
    plt.plot(range(len(train_losses)), train_losses, label='Train Loss')
    plt.plot(range(len(val_losses)), val_losses, label='Val Loss')
    plt.xlabel('iter_count')
    plt.ylabel('Loss')
    plt.legend()
    title = 'PB20081590'
    plt.title(title)
    # save the image
    plt.savefig('losses.png')

# define the hyperparameters
batch_size = 16
block_size = 512
max_iters = 5500  # set the number of training iterations as you like
eval_interval = 50
learning_rate = 4e-4
device = "cuda" if torch.cuda.is_available() else "cpu"
eval_iters = 200
n_embd = 64
n_heads = 8
n_layers = 7
dropOut = 0.2
trained = False
# read the dataset
with open("../data/input.txt", "r", encoding="utf-8") as f:
    text = f.read()
chars = sorted(list(set(text)))

# initialize the vocabulary
tokenizer = char_tokenizer(chars)
encode = tokenizer.encode
decode = tokenizer.decode
n_vocab = tokenizer.n_vocab

# separate the dataset into train and validation
train_data = torch.tensor(encode(text[: -len(text) // 10]), dtype=torch.long)
val_data = torch.tensor(encode(text[-len(text) // 10:]), dtype=torch.long)

# define the model
if trained == False:
  model = Transformer().to(device)
  train(model)
  torch.save(model.state_dict(),'./checkpoint.pth')
else:
  model = Transformer()
  model.load_state_dict(torch.load('./checkpoint.pth'))
  model = model.to(device)
# torch.save(model.state_dict(), "1.pth")
generate(model)
