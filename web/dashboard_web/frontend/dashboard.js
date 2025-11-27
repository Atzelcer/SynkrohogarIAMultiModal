/* Obtener URLs de ambas cámaras */
fetch("http://127.0.0.1:8000/url_camara")
    .then(r => r.json())
    .then(d => {
        // Cámara de la casa (ya está en el HTML como camCasa)
        if (d.casa) {
            document.getElementById("camCasa").src = d.casa;
            console.log("URL cámara casa:", d.casa);
        }
        console.log("URLs cámaras:", d);
    })
    .catch(err => console.error("Error obteniendo URLs:", err));

/* WS */
let ws = new WebSocket("ws://127.0.0.1:8000/ws_estado");

ws.onmessage = (m) => {
    let d = JSON.parse(m.data);

    updateTextWithAnimation("gesto", d.gesto || "");
    updateTextWithAnimation("comando", d.estado || "");
    updateTextWithAnimation("frase", d.frase || "");

    let audio = document.getElementById("audioIndicator");

    if (d.audio === "on") {
        audio.classList.remove("audio-off");
        audio.classList.add("audio-on");
    } else {
        audio.classList.remove("audio-on");
        audio.classList.add("audio-off");
    }
};

ws.onerror = (error) => {
    console.error("WebSocket error:", error);
};

ws.onclose = () => {
    console.log("WebSocket connection closed. Attempting to reconnect...");
    setTimeout(() => {
        ws = new WebSocket("ws://127.0.0.1:8000/ws_estado");
    }, 3000);
};

function updateTextWithAnimation(id, newText) {
    const element = document.getElementById(id);
    if (element && element.innerText !== newText) {
        element.style.opacity = "0";
        element.style.transform = "translateY(-10px)";
        
        setTimeout(() => {
            element.innerText = newText;
            element.style.opacity = "1";
            element.style.transform = "translateY(0)";
        }, 200);
    }
}

const nodes = [
    document.getElementById("node1"),
    document.getElementById("node2"),
    document.getElementById("node3"),
    document.getElementById("node4")
];

let idx = 0;

function animatePipeline() {
    nodes.forEach(n => n.classList.remove("active"));
    nodes[idx].classList.add("active");
    idx = (idx + 1) % nodes.length;
}

setInterval(animatePipeline, 1500);
animatePipeline();

/* Partículas */
const canvas = document.getElementById("particlesCanvas");
const ctx = canvas.getContext("2d");

let particles = [];
let mouse = { x: null, y: null, radius: 150 };

function resize() {
    canvas.width = window.innerWidth;
    canvas.height = window.innerHeight;
    initParticles();
}
resize();
window.onresize = resize;

window.addEventListener('mousemove', (e) => {
    mouse.x = e.x;
    mouse.y = e.y;
});

function initParticles() {
    particles = [];
    const numberOfParticles = Math.min(Math.floor((canvas.width * canvas.height) / 9000), 120);
    
    for (let i = 0; i < numberOfParticles; i++) {
        particles.push({
            x: Math.random() * canvas.width,
            y: Math.random() * canvas.height,
            r: Math.random() * 2.5 + 0.5,
            dx: (Math.random() - .5) * .4,
            dy: (Math.random() - .5) * .4,
            originalDx: (Math.random() - .5) * .4,
            originalDy: (Math.random() - .5) * .4
        });
    }
}

function drawParticles() {
    ctx.clearRect(0, 0, canvas.width, canvas.height);

    particles.forEach((p, i) => {
        ctx.beginPath();
        ctx.arc(p.x, p.y, p.r, 0, Math.PI * 2);
        
        const gradient = ctx.createRadialGradient(p.x, p.y, 0, p.x, p.y, p.r * 3);
        gradient.addColorStop(0, "rgba(0,255,255,0.8)");
        gradient.addColorStop(1, "rgba(0,255,255,0)");
        
        ctx.fillStyle = gradient;
        ctx.shadowBlur = 15;
        ctx.shadowColor = "#00faff";
        ctx.fill();

        let dx = mouse.x - p.x;
        let dy = mouse.y - p.y;
        let distance = Math.sqrt(dx * dx + dy * dy);
        
        if (distance < mouse.radius && mouse.x !== null) {
            let forceDirectionX = dx / distance;
            let forceDirectionY = dy / distance;
            let force = (mouse.radius - distance) / mouse.radius;
            
            p.dx -= forceDirectionX * force * 0.6;
            p.dy -= forceDirectionY * force * 0.6;
        } else {
            if (p.dx !== p.originalDx) p.dx += (p.originalDx - p.dx) * 0.05;
            if (p.dy !== p.originalDy) p.dy += (p.originalDy - p.dy) * 0.05;
        }

        p.x += p.dx;
        p.y += p.dy;

        if (p.x < 0 || p.x > canvas.width) p.dx *= -1;
        if (p.y < 0 || p.y > canvas.height) p.dy *= -1;

        for (let j = i + 1; j < particles.length; j++) {
            let p2 = particles[j];
            let dx = p.x - p2.x;
            let dy = p.y - p2.y;
            let distance = Math.sqrt(dx * dx + dy * dy);

            if (distance < 100) {
                ctx.beginPath();
                ctx.strokeStyle = `rgba(0, 255, 255, ${0.15 * (1 - distance / 100)})`;
                ctx.lineWidth = 0.5;
                ctx.moveTo(p.x, p.y);
                ctx.lineTo(p2.x, p2.y);
                ctx.stroke();
            }
        }
    });

    requestAnimationFrame(drawParticles);
}

initParticles();
drawParticles();
