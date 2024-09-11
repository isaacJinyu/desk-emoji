import numpy as np
from faster_whisper import WhisperModel
from openai import OpenAI
import pyttsx3


client = OpenAI(
    base_url = "https://www.apigptopen.xyz/v1",
    api_key = "sk-OBSavwoMHeYoDDr884894eD58f8d4097A415FeEf881aB47e"
)

rule_text = """
You are a small size desktop robot.
Your name is 'Open-Looi'.
Your gender is male.
Your age is 1.
You are funny and lovely.
Response in short and humor, without emoji.
"""

samplerate = 16000
block_duration = 0.5  # 每个音频块的持续时间（秒）
block_size = int(samplerate * block_duration)
silence_threshold = 0.02
silence_duration_threshold = 3.0  # 停顿阈值（秒）

model_size = "base"
model = WhisperModel(model_size, device="cpu", compute_type="int8")

audio_buffer = np.zeros((0,), dtype=np.float32)
silence_duration = 0.0
is_capturing = False
print("Initialization complete. Waiting to detect sound...")


def get_completion(prompt, model="gpt-3.5-turbo", temperature=0):
    messages = [
        {"role": "system", "content": rule_text},
        {"role": "user", "content": prompt}
    ]
    response = client.chat.completions.create(
        model=model,
        messages=messages,
        temperature=temperature,
    )
    return response.choices[0].message.content


def speak_text(text):
    engine = pyttsx3.init()
    try:
        engine.endLoop()
        del engine
        engine = pyttsx3.init()
    except:
        engine.startLoop()
        voices = engine.getProperty('voices')
        engine.setProperty('voice', voices[9].id)
        engine.say(text)
