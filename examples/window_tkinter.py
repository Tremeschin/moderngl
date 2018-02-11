import tkinter as tk

import moderngl
import numpy as np

from tkinter_framebuffer import FramebufferImage

ctx = moderngl.create_standalone_context()

prog = ctx.program(
    vertex_shader='''
        #version 330

        in vec2 in_vert;
        in vec3 in_color;

        out vec3 v_color;

        void main() {
            v_color = in_color;
            gl_Position = vec4(in_vert, 0.0, 1.0);
        }
    ''',
    fragment_shader='''
        #version 330

        in vec3 v_color;

        out vec3 f_color;

        void main() {
            f_color = v_color;
        }
    ''',
)


def vertices():
    x = np.linspace(-1.0, 1.0, 50)
    y = np.random.rand(50) - 0.5
    r = np.ones(50)
    g = np.zeros(50)
    b = np.zeros(50)
    return np.dstack([x, y, r, g, b])


vbo = ctx.buffer(vertices().astype('f4').tobytes())
vao = ctx.simple_vertex_array(prog, vbo, 'in_vert', 'in_color')


def update(*args):
    print(args)
    with tkfbo:
        ctx.clear()
        vbo.write(vertices().astype('f4').tobytes())
        vao.render(moderngl.LINE_STRIP)


root = tk.Tk()
tkfbo = FramebufferImage(root, ctx, (512, 512))

lbl = tk.Label(root, image=tkfbo)
lbl.bind("<ButtonPress-1>", update)
lbl.bind("<ButtonRelease-1>", update)
lbl.bind('<Motion>', update)
lbl.pack()

btn = tk.Button(root, text='Hello', command=update)
btn.pack()

root.mainloop()