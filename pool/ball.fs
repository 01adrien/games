#version 330

in vec2 fragTexCoord;
in vec4 fragColor;


uniform sampler2D texture0; // requis par raylib
uniform float angle;      // angle de rotation de la stripe
uniform vec4 ballColor;   // couleur de la boule
uniform int striped;      // 1 = rayée, 0 = pleine

out vec4 finalColor;

void main()
{
    // centre = (0,0), bords = -0.5 à 0.5
    vec2 uv = fragTexCoord - vec2(0.5);
    float dist = length(uv);

    // hors du cercle → transparent
    if (dist > 0.5)
    {
        finalColor = vec4(0.0);
        return;
    }

    if (striped == 1)
    {
        // rotation de l'axe de la stripe
        float c = cos(angle);
        float s = sin(angle);
        vec2 rotated = vec2(uv.x * c - uv.y * s, uv.x * s + uv.y * c);

        float stripeWidth = 0.25;

        if (abs(rotated.y) < stripeWidth)
            finalColor = ballColor;
        else
            finalColor = vec4(1.0, 1.0, 1.0, 1.0); // blanc
    }
    else
    {
        finalColor = ballColor;
    }

    // contour
    if (dist > 0.47)
    {
        finalColor = mix(finalColor, vec4(0.0, 0.0, 0.0, 1.0), 0.3);
        return;
    }

    // highlight
    vec2 hlCenter = vec2(-0.15, -0.15);
    if (length(uv - hlCenter) < 0.1)
    {
        finalColor = mix(finalColor, vec4(1.0), 0.7);
        return;
    }
}