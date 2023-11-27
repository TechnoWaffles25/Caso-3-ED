function buscarLibros() {
    const frase = document.getElementById('fraseInput').value;
    fetch('http://localhost:8089', {  // Asegúrate de usar el puerto correcto
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify({ frase: frase })
    })
    .then(response => response.json())
    .then(data => mostrarResultados(data))
    .catch(error => console.error('Error:', error));
}

function mostrarResultados(data) {
    const resultadosDiv = document.getElementById('resultados');
    resultadosDiv.innerHTML = '';  // Limpiar resultados anteriores

    data.forEach(libro => {
        const libroDiv = document.createElement('div');
        libroDiv.innerHTML = `
            <h2>${libro.titulo} - ${libro.autor}</h2>
            <p>${libro.extracto}</p>
            <p>Página: ${libro.pagina}</p>
            <p>Sentimiento: ${libro.sentimiento}</p>
            <img src="${libro.imagen}" alt="Imagen del libro">
        `;
        resultadosDiv.appendChild(libroDiv);
    });
}