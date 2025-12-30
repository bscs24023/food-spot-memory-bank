from flask import Flask, render_template, request, redirect, url_for
import py_foodspot  # The compiled Pybind11 module from C++ backend

app = Flask(__name__)

current_user = None


@app.route("/", methods=["GET", "POST"])
def index():
    global current_user
    error = ""
    if request.method == "POST":
        username = request.form["username"]
        if py_foodspot.user_exists(username):
            current_user = username
            return redirect(url_for("dashboard"))
        else:
            error = "User not found. Please register."
    return render_template("index.html", error=error)

@app.route("/register", methods=["GET", "POST"])
def register():
    global current_user
    message = ""
    if request.method == "POST":
        username = request.form["username"]
        if py_foodspot.create_user(username):
            current_user = username
            message = "Registration successful!"
            return redirect(url_for("dashboard"))
        else:
            message = "User already exists!"
    return render_template("index.html", error=message)

@app.route("/dashboard")
def dashboard():
    if not current_user:
        return redirect(url_for("index"))
    stats = py_foodspot.get_user_stats(current_user)
    return render_template("dashboard.html", user=current_user, stats=stats)

@app.route("/add_restaurant", methods=["GET", "POST"])
def add_restaurant():
    if not current_user:
        return redirect(url_for("index"))
    message = ""
    if request.method == "POST":
        name = request.form["name"]
        location = request.form["location"]
        cuisines = request.form.getlist("cuisines")
        rating = float(request.form["rating"])
        avg_price = float(request.form["avg_price"])
        notes = request.form["notes"]
        dishes = []

        dish_name = request.form.get("dish_name", "")
        dish_rating = float(request.form.get("dish_rating", "0"))
        dish_price = float(request.form.get("dish_price", "0"))
        if dish_name:
            dishes.append((dish_name, dish_rating, dish_price))

        success = py_foodspot.add_restaurant(
            current_user, name, location, cuisines, rating, avg_price, dishes, notes
        )
        message = "Restaurant added successfully!" if success else "Error adding restaurant."
    return render_template("add_restaurant.html", message=message)

@app.route("/restaurant_list")
def restaurant_list():
    if not current_user:
        return redirect(url_for("index"))
    restaurants = py_foodspot.get_user_restaurants(current_user)
    return render_template("restaurant_list.html", restaurants=restaurants)

@app.route("/recommendations")
def recommendations():
    if not current_user:
        return redirect(url_for("index"))
    friend_recs = py_foodspot.get_friend_recommendations(current_user)
    city = request.args.get("city", "")
    city_recs = py_foodspot.get_city_recommendations(current_user, city) if city else []
    return render_template("recommendations.html", friend_recs=friend_recs, city_recs=city_recs)

@app.route("/alerts")
def alerts():
    if not current_user:
        return redirect(url_for("index"))
    alerts = py_foodspot.get_alerts(current_user)
    return render_template("alerts.html", alerts=alerts)

@app.route("/friends", methods=["GET", "POST"])
def friends():
    if not current_user:
        return redirect(url_for("index"))
    message = ""
    friends_list = py_foodspot.get_friends(current_user)
    if request.method == "POST":
        friend_name = request.form["friend_name"]
        success = py_foodspot.add_friend(current_user, friend_name)
        message = "Friend added!" if success else "Error adding friend."
    return render_template("friends.html", friends=friends_list, message=message)

@app.route("/preferences")
def preferences():
    if not current_user:
        return redirect(url_for("index"))
    prefs = py_foodspot.get_user_preferences(current_user)
    return render_template("preferences.html", preferences=prefs)

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=True)
