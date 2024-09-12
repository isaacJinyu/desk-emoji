import numpy as np
import pyttsx3
from openai import OpenAI
from faster_whisper import WhisperModel
from langchain.memory import ConversationBufferMemory


client = OpenAI(
    base_url = "https://www.apigptopen.xyz/v1",
    api_key = "sk-OBSavwoMHeYoDDr884894eD58f8d4097A415FeEf881aB47e"
)

language = 'en'

samplerate = 16000
block_duration = 0.5
block_size = int(samplerate * block_duration)
silence_threshold = 0.02
silence_duration_threshold = 3.0

model_size = "base"
print("Loading model...")
model = WhisperModel(model_size, device="cpu", compute_type="int8")
memory = ConversationBufferMemory(memory_key="chat_history")
max_memory = 30

audio_buffer = np.zeros((0,), dtype=np.float32)
silence_duration = 0.0
is_capturing = False
print("Initialization done.")
print("Listening...")


def get_completion(model="gpt-3.5-turbo", temperature=0, history_messages=[], prompt=''):
    rule_text = """
You are a small size desktop robot.
You are funny and lovely.
Your name is 'Desk-Emoji'.
Your gender is male.
Your age is 1.
Your skin is black.
Your eyes are blue.
You have no hands and legs, stick on the desk.
I am your master.
Responses in short, with humorous, without emoji.
"""
    messages = [{"role": "system", "content": rule_text}]
    for message in history_messages:
        if message.startswith('Human:'):
            role = "user"
            content = message.replace('Human:', '').strip()
        elif message.startswith('AI:'):
            role = "assistant"
            content = message.replace('AI:', '').strip()
        else:
            continue
        messages.append({"role": role, "content": content})
    messages.append({"role": "user", "content": prompt})

    response = client.chat.completions.create(
        model=model,
        messages=messages,
        temperature=temperature,
    )
    return response.choices[0].message.content


def chat(question):
    if not question: return
    print(f"You: {question}")
    memory_content = memory.load_memory_variables(inputs={})
    history = memory_content.get('chat_history', [])
    history_messages = history.split('\n')[max_memory*-2:]
    answer = get_completion(temperature=1, history_messages=history_messages, prompt=question)
    memory.chat_memory.add_user_message(question)
    memory.chat_memory.add_ai_message(answer)
    print(f"Bot: {answer}\n")
    speak_text(answer)


def speak_text(text):
    engine = pyttsx3.init()
    try:
        engine.endLoop()
        del engine
        engine = pyttsx3.init()
    except:
        voices = engine.getProperty('voices')
        voice_id = 9 if language == 'en' else 135
        engine.setProperty('voice', voices[voice_id].id)
        engine.startLoop()
        engine.say(text)
